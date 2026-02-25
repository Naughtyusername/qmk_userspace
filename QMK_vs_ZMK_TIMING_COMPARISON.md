# QMK vs ZMK: Tap-Hold Timing Deep Dive

## The Fundamental Architectural Difference

### QMK: Sequential Decision Model
- **One active tap-hold key** at a time (`static keyrecord_t tapping_key`)
- Subsequent tap-holds are **buffered** (`waiting_buffer[8]`) and queued for later
- **State machine**: One key progresses through states (undecided → tap → hold/tap)
- **Result**: Predictable, but keys after the first tap-hold are delayed

### ZMK: Parallel Decision Model
- **All tap-holds can be deciding simultaneously**
- Each key has its own state/timer tracking
- **Result**: More responsive, but more complex state management

---

## Example: Three-Key Sequence

### Scenario: User presses `SFT_T(A)` then `LT(2, B)` then `KC_C` (all within 100ms)

#### QMK Flow

```
Time  Event             QMK State                              Action Taken
───────────────────────────────────────────────────────────────────────────
0ms   SFT_T(A) down     tapping_key = SFT_T(A)                Undecided, waiting
                        tap.count = 0, tap.interrupted = false

50ms  LT(2, B) down     tapping_key = SFT_T(A)  [unchanged]  NOT processed
                        waiting_buffer[0] = LT(2, B) down     Buffered

100ms KC_C down         tapping_key = SFT_T(A)  [unchanged]  NOT processed
                        waiting_buffer[1] = KC_C down         Buffered

150ms SFT_T(A) released tapping_key.event.pressed = false     SFT_T(A) settles
                        Tapping term expired? 150 > 200? NO   as TAP
                        → tap.count = 1
                        → process_record(SFT_T)
                        → Sends 'a' to host

      [Buffer draining] waiting_buffer_tail = 0              Process LT(2, B) down
                        → tapping_key = LT(2, B)             Becomes new undecided
                        → waiting_buffer_tail = 1
                        → LT decides: wait until TAPPING_TERM

200ms LT(2, B) still    tapping_key = LT(2, B)               Still undecided
      undecided         waiting_buffer[1] = KC_C down        KC_C waiting

250ms KC_C released     (KC_C release would be buffered)

280ms LT(2, B) released tapping_key.event.pressed = false
                        Tapping term: 280 - 50 = 230ms > 200  Settles as HOLD
                        → tap.count = 0 (hold)
                        → process_record(LT(2, B))
                        → Activates Layer 2

      [Buffer draining] waiting_buffer[1] = KC_C down
                        → process_record(KC_C) with Layer 2 active
                        → (Depends on Layer 2 mapping of KC_C)

Result: 'a', then (Layer 2 version of KC_C)
Timeline: 'a' sent immediately at 150ms; large delay before KC_C processed
```

#### ZMK Flow (for comparison)

```
Time  Event             ZMK State                              Action Taken
───────────────────────────────────────────────────────────────────────────
0ms   SFT_T(A) down     A.state = undecided                   Both decide in parallel
                        B.state = (not yet pressed)

50ms  LT(2, B) down     A.state = undecided
                        B.state = undecided                   Both waiting

100ms KC_C down         A.state = undecided
                        B.state = undecided
                        KC_C: regular key (not tap-hold)      KC_C buffered, waiting

150ms SFT_T(A) released A.state = released after 150ms < TT   A settles as TAP
                        → Sends 'a'
                        B is still undecided (no requirement) B keeps deciding

200ms [LT Term expires] B.state = released after ~150ms < TT  B settles as TAP
                        B.tap.count = 1
                        → Sends 'b'

      [Parallel result] KC_C from buffer processed            Sends 'c'

Result: 'a', 'b', 'c' (in order, as if typed naturally)
Timeline: All three keys potentially processed faster; no stalling
```

**Key insight**: ZMK starts processing KC_C immediately after LT(2, B)'s decision is made, rather than waiting for a sequential queue to empty. QMK's buffer prevents state explosion but adds delay.

---

## Timing Term Differences

### QMK: TAPPING_TERM Interpretation

**TAPPING_TERM** is the timeout for deciding **one specific key**. But the behavior depends on three factors:

1. **Decision mode** (default, PERMISSIVE_HOLD, HOLD_ON_OTHER_KEY_PRESS)
2. **Whether other keys are pressed**
3. **QUICK_TAP_TERM and FLOW_TAP_TERM interactions**

**Example**: Setting TAPPING_TERM=250 doesn't mean "wait 250ms" — it means "here's how long before hold is triggered IF nothing else happens."

```c
#define TAPPING_TERM 250

// Timeline:
0ms:   SFT_T(KC_A) down
50ms:  KC_B down
       Default mode: KC_B buffered, SFT_T still waiting
150ms: SFT_T(KC_A) released
       150ms < 250ms → Settles as TAP (key released before timeout)
```

### ZMK: Tapping Term Interpretation

**Tapping Term** (e.g., 200ms) is straightforward: the window in which pressing other keys doesn't change the decision.

```c
tapping-term-ms = <200>;

// Timeline:
0ms:   mod-tap(shift, a) pressed
50ms:  b pressed
       require-prior-idle-ms check: Has it been 200ms since b was processed?
       If NO: mod-tap might not hold (depends on flavor)
150ms: mod-tap released
       150ms < 200ms → Settles as TAP (too fast)
```

ZMK's logic is more **event-driven** and **stateful-per-key**, whereas QMK's is **single-state-machine** with **decision modes**.

---

## FLOW_TAP_TERM: QMK's Speed Optimization

QMK added `FLOW_TAP_TERM` to solve a problem ZMK doesn't have: **typing speed feedback**.

### Why QMK Needed FLOW_TAP_TERM

In QMK's single-state model, if you're typing fast:

```
0ms:   'a' pressed
50ms:  'a' released
80ms:  LSFT_T(KC_S) pressed
       Default: Now waiting until time=280ms (200ms TAPPING_TERM from 80ms)
       User has to wait 200ms before 's' is sent
       → Feels laggy during fast typing
```

**ZMK avoids this** because it decides on the first key earlier, then moves to the next key's decision immediately.

### FLOW_TAP_TERM Solution

When a new tap-hold key is pressed:
1. Check: "How much idle time since the previous key?"
2. If idle time < FLOW_TAP_TERM (e.g., 150ms), **immediately settle as TAP**
3. This is ZMK-like behavior: "User is typing fast; don't wait"

```c
#define FLOW_TAP_TERM 150

Timeline:
0ms:   'a' pressed
50ms:  'a' released
80ms:  LSFT_T(KC_S) pressed
       idle_time = 80 - 50 = 30ms < 150ms
       → IMMEDIATELY settle as TAP (don't wait 200ms)
       → Send 's' now
       Result: Fast response
```

**Key insight**: FLOW_TAP_TERM is QMK's attempt to add "smart fast typing detection" to a fundamentally sequential system.

---

## require-prior-idle-ms (ZMK) vs QMK Equivalents

### ZMK's require-prior-idle-ms

```yaml
behaviors {
    ht: hold_tap {
        label = "home row mods";
        #binding-cells = <2>;
        tapping-term-ms = <200>;
        require-prior-idle-ms = <100>;  // Key point!
        flavor = "balanced";
        bindings = <&kp>, <&kp>;
    };
};
```

**Effect**: Tap-hold key will NOT settle as hold unless:
- The key is held past tapping-term, AND
- 100ms has passed since ANY other key was last pressed/released

**Timeline**:
```
0ms:   'a' down, 'a' up (at 50ms)
100ms: LSFT_T(KC_S) down  ← 50ms idle since 'a'
       Is 50ms >= 100ms? NO
       → Can't hold yet; only tap allowed

150ms: LSFT_T(KC_S) still held
       Is (150-50)=100ms >= 100ms? YES
       → Now can hold if condition met
```

### QMK Equivalent: PERMISSIVE_HOLD + CHORDAL_HOLD + Timing

QMK doesn't have a direct idle tracker, but can achieve similar effect:

```c
#define TAPPING_TERM 250
#define TAPPING_TERM_PER_KEY
#define PERMISSIVE_HOLD_PER_KEY
#define CHORDAL_HOLD

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    // Extend the window to give time for idle detection via PERMISSIVE_HOLD
    switch (keycode) {
        case LSFT_T(KC_A):
        case LCTL_T(KC_S):
        case LALT_T(KC_D):
        case LGUI_T(KC_F):
            return 250;
        default:
            return TAPPING_TERM;
    }
}

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    // Only enable for home row mods
    return (record->event.key.row == 3);  // Home row
}
```

**How this works**:
1. **Longer TAPPING_TERM (250ms)**: Extends decision window
2. **PERMISSIVE_HOLD**: Only considers it a "hold" if nested press-release detected
3. **CHORDAL_HOLD**: Prevents same-hand accidental holds (rolling keys)

**Result**: Similar to require-prior-idle, but not identical:
- ZMK: "Don't hold unless idle for 100ms"
- QMK: "Don't hold unless other key is complete (press+release), or opposite hand"

### Alternative: FLOW_TAP_TERM (Opposite Philosophy)

```c
#define FLOW_TAP_TERM 150
```

**Effect**: "During typing flow (< 150ms idle), force tap. After pause (> 150ms), allow hold."

This is the inverse of require-prior-idle:
- **Typing fast** → TAP (disables hold)
- **Pause & hold** → HOLD (enables hold)

### Another Alternative: HOLD_ON_OTHER_KEY_PRESS_PER_KEY + Timing

```c
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY

bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LSFT_T(KC_A):
        case LCTL_T(KC_S):
        case LALT_T(KC_D):
        case LGUI_T(KC_F):
            return true;  // Space bar mods: hold immediately on other press
        default:
            return false;  // Others: default behavior
    }
}
```

**Effect**: "As soon as another key touches down, settle as hold"

This is most aggressive and least like require-prior-idle.

---

## Cascading Tap-Holds: How Each System Handles It

### Scenario: SFT_T(A) then LT(2, B) then KC_C, all within 150ms

#### QMK Cascading Prevention

```
State: tapping_key = SFT_T(A)
       waiting_buffer has: [LT(2,B) down, LT(2,B) up, KC_C ...]

Process flow:
1. SFT_T(A) decided → tap.count = 1
2. Drain buffer: LT(2, B) down → tapping_key = LT(2, B)
3. LT(2, B) decided → tap.count = 0 (hold)
4. Drain buffer: KC_C → process while layer active
5. LT(2, B) up → release

Result: Prevents cascading via strict FIFO queue
Cost: Key 2 & 3 delayed until key 1 is decided
```

**Buffer overflow** (> 8 events): Panic, clear all, start over

#### ZMK Cascading Prevention

```
State: A.state = undecided, B.state = undecided, C queued
       Each key has own timer and decision logic

Process flow:
1. A decided (T+50ms) → tap.count = 1
2. B still undecided (parallel), C waiting
3. B decided (T+200ms) → tap.count = 0 (hold)
4. C processed with layer active

Result: Parallel decision trees; no queue bottleneck
Cost: More complex state tracking
```

**Overflow**: Less likely; each key manages its own state

---

## Configuration Complexity Comparison

### ZMK Configuration
```yaml
behaviors {
    ht: hold_tap {
        tapping-term-ms = <200>;
        quick-tap-ms = <120>;
        require-prior-idle-ms = <150>;
        flavor = "balanced";  // or "tap-preferred" or "hold-preferred"
        bindings = <&kp>, <&kp>;
    };
};
```

**Keys**:
- **tapping-term-ms**: Straightforward timeout
- **quick-tap-ms**: Double-tap behavior
- **require-prior-idle-ms**: Idle tracking
- **flavor**: Positional logic (on-press vs on-release)

**Result**: ~4 levers to configure per behavior

### QMK Configuration

```c
#define TAPPING_TERM 200
#define QUICK_TAP_TERM 120
#define TAPPING_TERM_PER_KEY
#define QUICK_TAP_TERM_PER_KEY
#define PERMISSIVE_HOLD
#define PERMISSIVE_HOLD_PER_KEY
#define HOLD_ON_OTHER_KEY_PRESS
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
#define CHORDAL_HOLD
#define FLOW_TAP_TERM 150
#define SPECULATIVE_HOLD
#define RETRO_TAPPING

// Plus callbacks:
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) { ... }
uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) { ... }
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) { ... }
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) { ... }
bool get_chordal_hold(...) { ... }
uint16_t get_flow_tap_term(...) { ... }
bool get_speculative_hold(...) { ... }
bool get_retro_tapping(...) { ... }
```

**Keys**:
- TAPPING_TERM: Base timeout
- QUICK_TAP_TERM: Double-tap timing
- Decision modes: 3 choices (default, permissive, hold-on-other)
- CHORDAL_HOLD: Spatial logic
- FLOW_TAP_TERM: Speed-based override
- SPECULATIVE_HOLD: Apply mods early
- RETRO_TAPPING: Send tap if held past term with no other key

**Result**: ~13 options + 8 callbacks = high complexity

**Why?** QMK accumulated features over time; many are optimizations for specific use cases rather than core design.

---

## Performance Implications

### QMK
- **Waiting buffer**: Fixed 8-event buffer; uses little memory
- **Single tapping_key**: Simple state, small footprint
- **CPU overhead**: Minimal; just checking timers and buffer queue
- **Latency**: May add delay while waiting for tap-hold decision

### ZMK
- **Per-key state tracking**: More memory for each tap-hold key
- **Parallel timers**: Multiple timers running simultaneously
- **CPU overhead**: More decision logic per key
- **Latency**: Can process keys faster due to parallelism

**Trade-off**: QMK is leaner; ZMK is more responsive but requires more state management.

---

## Why QMK's Timing Feels Different to ZMK Users

1. **Cascading model**: QMK queues, ZMK decides in parallel
2. **Decision timing**: QMK has three modes; ZMK has flavor + require-prior-idle
3. **Fast typing**: QMK added FLOW_TAP_TERM; ZMK built it in via require-prior-idle
4. **Spatial awareness**: QMK added CHORDAL_HOLD as an option; ZMK doesn't have it
5. **Configuration**: QMK has more levers; ZMK has fewer but tightly coupled

**Bottom line**: If you're used to ZMK's behavior, QMK feels like you're waiting for cascading decisions. Adjusting TAPPING_TERM alone won't fix it; you need to enable appropriate decision modes (PERMISSIVE_HOLD + CHORDAL_HOLD + FLOW_TAP_TERM) to get similar feel.

---

## Recommended QMK Config for ZMK Users

If you want QMK to feel like ZMK:

```c
// config.h
#define TAPPING_TERM 200
#define QUICK_TAP_TERM 150
#define TAPPING_TERM_PER_KEY
#define PERMISSIVE_HOLD_PER_KEY
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
#define CHORDAL_HOLD
#define FLOW_TAP_TERM 150

// keymap.c
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    // Home row mods: longer term to allow nested taps
    if (record->event.key.row == 3) {
        return 220;
    }
    return TAPPING_TERM;
}

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    // Only for home row mods; allows nested taps to trigger hold
    return record->event.key.row == 3;
}

bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    // Not enabled per-key; use default (permissive)
    return false;
}
```

**Effect**:
- TAPPING_TERM=200 + PERMISSIVE_HOLD = Similar to ZMK "balanced" flavor
- CHORDAL_HOLD = Prevents accidental mods on same-hand rolls (similar to require-prior-idle)
- FLOW_TAP_TERM=150 = Fast typing boost (similar to ZMK's behavior during idle)
- PER_KEY settings = Fine-tuning per position (similar to ZMK behaviors)

---
