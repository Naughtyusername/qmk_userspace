# QMK Tap-Hold Timing System - Comprehensive Research

## Executive Summary

QMK's tap-hold system is fundamentally different from ZMK's in critical ways:

1. **Single simultaneous tap-hold key**: QMK maintains ONE active tapping key at a time, with others buffered
2. **No per-key override priority**: `get_tapping_term()` returns the exact timeout for that key, but all other tap-hold keys with different timing stack in a queue
3. **Three distinct decision modes** that affect timing and when the decision is made
4. **Flow Tap is a "fast typing boost"** not a behavior override - it disables the hold action during typing flow
5. **CHORDAL_HOLD only affects timing decisions**, not the actual tapping term value
6. **Cascading is prevented through buffering**, not through require-prior-idle logic

---

## 1. TAPPING_TERM, QUICK_TAP_TERM, and FLOW_TAP_TERM Interaction

### TAPPING_TERM (Default: 200ms)
**File**: `/home/naughty/qmk/qmk_firmware/quantum/action_tapping.h:21-23`

```c
#ifndef TAPPING_TERM
#    define TAPPING_TERM 200
#endif
```

This is the primary window during which a tap-hold key can decide between tap and hold. The decision flow:

1. Key is pressed → enters "tapping" state (not yet decided)
2. If key is released **before TAPPING_TERM expires** → settles as a tap
3. If key is held **past TAPPING_TERM** → settles as a hold
4. If another key is pressed during TAPPING_TERM → behavior depends on the **decision mode** (see section 3)

**Macro extraction**:
```c
#define WITHIN_TAPPING_TERM(e) (TIMER_DIFF_16(e.time, tapping_key.event.time) < GET_TAPPING_TERM(...))
```

This is checked in `action_tapping.c:305` to determine if an event falls within the tapping window.

### QUICK_TAP_TERM (Default: TAPPING_TERM)
**File**: `/home/naughty/qmk/qmk_firmware/quantum/action_tapping.h:26-28`

```c
#if !defined(QUICK_TAP_TERM) || QUICK_TAP_TERM > TAPPING_TERM
#    define QUICK_TAP_TERM TAPPING_TERM
#endif
```

**Purpose**: Controls auto-repeat behavior and tap-count sequences.

**When it applies** (from `action_tapping.c:602`):
```c
if (WITHIN_QUICK_TAP_TERM(event) && !tapping_key.tap.interrupted && tapping_key.tap.count > 0)
```

**Scenario**: User taps a key, releases it, then presses it again
- If second press happens **within QUICK_TAP_TERM** → treated as a sequential tap (tap.count increments)
- If second press happens **after QUICK_TAP_TERM** → treated as a new press (new tapping_key)

**Example from docs**:
```
SFT_T(KC_A) Down
SFT_T(KC_A) Up (sends 'a')
SFT_T(KC_A) Down (within QUICK_TAP_TERM)
(wait for tapping term to expire)
```
Result: 'a' is auto-repeated (tap behavior repeats), not shifted.

If QUICK_TAP_TERM = 0, the second press immediately activates shift (hold behavior).

### FLOW_TAP_TERM Interaction
**File**: `/home/naughty/qmk/qmk_firmware/quantum/action_tapping.h:144-212`

**Purpose**: Disables hold behavior during fast typing to reduce input lag.

**Critical distinction**: FLOW_TAP_TERM is **not** a timeout you wait for—it's a **retrospective check**. When a tap-hold key is pressed, QMK asks: "Did the last key come within FLOW_TAP_TERM of this press?"

**Flow Tap decision flow** (from `action_tapping.c:1032-1055`):
```c
static bool flow_tap_key_if_within_term(keyrecord_t *record, uint16_t prev_time) {
    const uint16_t idle_time = TIMER_DIFF_16(record->event.time, prev_time);
    if (flow_tap_expired || idle_time >= 500) {
        return false;
    }

    const uint16_t keycode = get_record_keycode(record, false);
    if (is_mt_or_lt(keycode)) {
        uint16_t term = get_flow_tap_term(keycode, record, flow_tap_prev_keycode);
        if (term > 500) {
            term = 500;  // Capped at 500ms
        }
        if (idle_time < term) {
            // Force immediate tap settlement
            record->tap.count = 1;
            registered_taps_add(record->event.key);
            process_record(record);
            return true;
        }
    }
    return false;
}
```

**Key points**:
1. `idle_time` is calculated from the **previous key's time** to the current tap-hold key's time
2. If `idle_time < FLOW_TAP_TERM`, the tap-hold key is **immediately settled as tapped** (tap.count = 1)
3. This happens **on key down**, before TAPPING_TERM would normally trigger
4. Maximum capped at 500ms regardless of configuration
5. Only applies when both current and previous keycodes are "flow tap keys" (alphas + space by default)

**Flow Tap vs TAPPING_TERM**:
- TAPPING_TERM: "How long to wait for a decision?"
- FLOW_TAP_TERM: "Was the last keypress recent? If yes, force this to be a tap now."

**When FLOW_TAP_TERM applies**:
- Called in `action_tapping.c:280` when starting a new tapping_key (if no tapping_key is active)
- Called in `action_tapping.c:618` when a new tap-hold key is pressed after the previous one released

**Timeline example** (TAPPING_TERM=200ms, FLOW_TAP_TERM=150ms):
```
0ms:    'a' pressed
50ms:   'a' released, flow_tap_prev_time = 50ms, flow_tap_prev_keycode = KC_A
80ms:   LSFT_T(KC_S) pressed
        idle_time = 80 - 50 = 30ms
        30ms < 150ms → FLOW_TAP immediately settles LSFT_T(KC_S) as a tap (sends 's')
        NOT waiting 200ms for TAPPING_TERM
```

---

## 2. Per-Key Tapping Term Behavior

### Global vs Per-Key Override

**File**: `/home/naughty/qmk/qmk_firmware/quantum/action_tapping.h:218-224`

```c
#if defined(TAPPING_TERM_PER_KEY) && !defined(NO_ACTION_TAPPING)
#    define GET_TAPPING_TERM(keycode, record) get_tapping_term(keycode, record)
#elif defined(DYNAMIC_TAPPING_TERM_ENABLE) && !defined(NO_ACTION_TAPPING)
#    define GET_TAPPING_TERM(keycode, record) g_tapping_term
#else
#    define GET_TAPPING_TERM(keycode, record) (TAPPING_TERM)
#endif
```

**How it works**:
1. Enable `#define TAPPING_TERM_PER_KEY` in `config.h`
2. Implement `get_tapping_term()` callback:
```c
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case SFT_T(KC_SPC):
            return TAPPING_TERM + 1250;  // 1450ms for space shift
        case LT(1, KC_GRV):
            return 130;  // 130ms for layer tap
        default:
            return TAPPING_TERM;
    }
}
```

**Critical behavior**: The function is called **every time** `GET_TAPPING_TERM` is needed:
- Line 27 in `action_tapping.c`: `WITHIN_TAPPING_TERM(e)` macro uses it
- Line 28: `WITHIN_QUICK_TAP_TERM(e)` also uses the tapping term value

**Does it override globally?** YES, but only for that specific key. Other keys still use TAPPING_TERM.

**Multiple tap-hold keys in sequence**:
```
Time  Event                           Status
0ms   SFT_T(KC_A) down              → tapping_key = SFT_T(KC_A), waiting for decision
80ms  LT(2, KC_B) down              → enqueued in waiting_buffer[0]
100ms LT(2, KC_B) released          → processed from waiting_buffer
150ms SFT_T(KC_A) still held
      Check: 150ms < GET_TAPPING_TERM(SFT_T(KC_A)) = 200ms? YES
      → SFT_T(KC_A) released       → settles as tap ('a')
```

The queue ensures **only one tap-hold key is actively being decided at a time**. Others are buffered.

---

## 3. Hold-Tap Flavors and Decision Modes

QMK doesn't call them "flavors"—it calls them **decision modes**. There are three, in increasing order of eagerness to hold:

### Mode 1: Default (Timeout-based)
**Behavior**: Hold is selected only when:
1. The tap-hold key is held **longer than TAPPING_TERM**, OR
2. TAPPING_TERM expires and the key is still held

**Other keys pressed during tapping term**: They are **buffered and delayed**, then processed after the tap-hold decision is made.

**Code location**: The default flow in `action_tapping.c:305-502` (the main `process_tapping()` function)

**Example**:
```
Timeline (TAPPING_TERM=200ms):
0ms:   LSFT_T(KC_A) down
50ms:  KC_B down          → KC_B buffered in waiting_buffer
60ms:  KC_B released      → Still buffered
70ms:  LSFT_T(KC_A) released
       → LSFT_T(KC_A) settles as TAP ('a')
       → KC_B from buffer processed as 'b'
       Result: "ab" (not "Shift+B")
```

### Mode 2: PERMISSIVE_HOLD
**Enabled with**: `#define PERMISSIVE_HOLD` in config.h

**Behavior**: Hold is selected when:
1. Default conditions (timeout), OR
2. Another key is **pressed AND released** (nested press) before TAPPING_TERM expires

**Key distinction**: Only recognizes "nested" sequences (both press and release of inner key before outer key releases), not "rolling" sequences.

**Code**: `action_tapping.c:369-377` checks `TAP_GET_PERMISSIVE_HOLD` condition

**Example** (PERMISSIVE_HOLD enabled):
```
NESTED (converts to hold):
0ms:   LSFT_T(KC_A) down
50ms:  KC_B down
60ms:  KC_B released      ← Inner key released before outer
150ms: LSFT_T(KC_A) released
       → LSFT_T(KC_A) settles as HOLD (Shift)
       → KC_B is processed while shift is active: "B"

ROLLING (still tap):
0ms:   LSFT_T(KC_A) down
50ms:  KC_B down
150ms: LSFT_T(KC_A) released  ← Outer released before inner
160ms: KC_B released
       → LSFT_T(KC_A) settles as TAP ('a')
       → KC_B is processed: 'b'
       Result: "ab"
```

### Mode 3: HOLD_ON_OTHER_KEY_PRESS
**Enabled with**: `#define HOLD_ON_OTHER_KEY_PRESS` in config.h

**Behavior**: Hold is selected when:
1. Default conditions (timeout), OR
2. Any other key is **pressed** (even if not released), before TAPPING_TERM expires

**Key distinction**: Reacts to the mere *press* of another key, not its release.

**Code**: `action_tapping.c:470-498` checks `TAP_GET_HOLD_ON_OTHER_KEY_PRESS` condition

**Example** (HOLD_ON_OTHER_KEY_PRESS enabled):
```
ROLLING (converts to hold):
0ms:   LSFT_T(KC_A) down
50ms:  KC_B down
150ms: LSFT_T(KC_A) released  ← Outer released before inner
160ms: KC_B released
       → LSFT_T(KC_A) settles as HOLD (Shift) at the moment KC_B was pressed
       → KC_B is processed while shift is active: "B"

NESTED (same as PERMISSIVE_HOLD):
0ms:   LSFT_T(KC_A) down
50ms:  KC_B down
60ms:  KC_B released
150ms: LSFT_T(KC_A) released
       → LSFT_T(KC_A) settles as HOLD (Shift)
       Result: "B"
```

### Per-Key Decision Mode Control
**File**: `action_tapping.h:239-245`, `action_tapping.c:113-116`

```c
#ifdef HOLD_ON_OTHER_KEY_PRESS_PER_KEY
__attribute__((weak)) bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    return false;
}
#endif

#ifdef PERMISSIVE_HOLD_PER_KEY
__attribute__((weak)) bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    return false;
}
#endif
```

**Usage**:
```c
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(1, KC_BSPC):
            return true;  // This key uses HOLD_ON_OTHER_KEY_PRESS
        default:
            return false;  // Others use default mode
    }
}
```

---

## 4. CHORDAL_HOLD Mechanics and Timing Interaction

**Enabled with**: `#define CHORDAL_HOLD` in config.h

**File**: `action_tapping.h:79-142` and `action_tapping.c:922-988`

### What CHORDAL_HOLD Does

It adds an additional **handedness check** during the tap-hold decision process. It does **NOT** change the tapping term itself.

**Core logic** (from `action_tapping.c:927-938`):
```c
bool get_chordal_hold_default(keyrecord_t *tap_hold_record, keyrecord_t *other_record) {
    if (tap_hold_record->event.type != KEY_EVENT || other_record->event.type != KEY_EVENT) {
        return true;  // Non-key events (combos) allow hold
    }

    char tap_hold_hand = chordal_hold_handedness(tap_hold_record->event.key);
    if (tap_hold_hand == '*') {
        return true;  // '*' (exempt keys) always allow hold
    }
    char other_hand = chordal_hold_handedness(other_record->event.key);
    return other_hand == '*' || tap_hold_hand != other_hand;
    // Returns true if hands are opposite OR either is exempt
}
```

### How It Affects Timing

**Scenario 1: Same-hand chord**
```
Time  Event
0ms:  SFT_T(KC_A) down (LEFT hand)
50ms: KC_C down (LEFT hand)       ← Same hand!
      get_chordal_hold() returns FALSE
      → Immediately settles as TAP
      Tapping term: ignored
      Result: "ac"
```

**Scenario 2: Opposite-hand chord**
```
Time  Event
0ms:  SFT_T(KC_A) down (LEFT hand)
50ms: KC_C down (RIGHT hand)      ← Opposite hand
      get_chordal_hold() returns TRUE
      → Not immediately settled
      → Respects TAPPING_TERM and other settings
      → Can become hold or tap depending on timing/mode
```

### CHORDAL_HOLD with HOLD_ON_OTHER_KEY_PRESS
```
Opposite hands:
0ms:  SFT_T(KC_A) down (LEFT)
50ms: KC_C down (RIGHT)          ← Settles as HOLD immediately
150ms: SFT_T(KC_A) released
       Result: "C" (shift was held)
```

### CHORDAL_HOLD with PERMISSIVE_HOLD
```
Opposite hands:
0ms:  SFT_T(KC_A) down (LEFT)
50ms: KC_C down (RIGHT)
60ms: KC_C released              ← Nested, settles as HOLD
150ms: SFT_T(KC_A) released
       Result: "C" (shift was held)

Opposite hands, rolling:
0ms:  SFT_T(KC_A) down (LEFT)
50ms: KC_C down (RIGHT)
150ms: SFT_T(KC_A) released      ← Released first
160ms: KC_C released
       PERMISSIVE_HOLD says: not nested, don't hold
       → Settles as TAP ('a')
       Result: "ac"
```

### Handedness Definition
```c
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM =
    LAYOUT(
        'L', 'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R', 'R',
        'L', 'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R', 'R',
        'L', 'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R', 'R',
                       'L', 'L', 'L',  'R', 'R', 'R'
    );
```

Or via function:
```c
char chordal_hold_handedness(keypos_t key) {
    if (key.col < MATRIX_COLS / 2) return 'L';
    return 'R';
}
```

---

## 5. Cascading Tap-Hold Issue and Prevention

### What Is Cascading?

When you press tap-hold key A, then tap-hold key B before A is decided, QMK must handle multiple simultaneous "undecided" keys.

### How QMK Prevents It: The Waiting Buffer

**File**: `action_tapping.c:131-133`, lines 657-673

```c
static keyrecord_t waiting_buffer[WAITING_BUFFER_SIZE] = {};
static uint8_t     waiting_buffer_head                 = 0;
static uint8_t     waiting_buffer_tail                 = 0;

bool waiting_buffer_enq(keyrecord_t record) {
    if (IS_NOEVENT(record.event)) {
        return true;
    }

    if ((waiting_buffer_head + 1) % WAITING_BUFFER_SIZE == waiting_buffer_tail) {
        ac_dprintf("waiting_buffer_enq: Over flow.\n");
        return false;
    }

    waiting_buffer[waiting_buffer_head] = record;
    waiting_buffer_head                 = (waiting_buffer_head + 1) % WAITING_BUFFER_SIZE;

    ac_dprintf("waiting_buffer_enq: ");
    debug_waiting_buffer();
    return true;
}
```

**Buffer size**: 8 key events (WAITING_BUFFER_SIZE = 8, line 35)

### The Single Active Tapping Key Model

**Core principle**: Only **ONE** tap-hold key can be actively "deciding" at a time.

**Flow** (from `action_tapping.c:148-200`):
```c
void action_tapping_process(keyrecord_t record) {
    if (process_tapping(&record)) {
        // Processed successfully
        if (IS_EVENT(record.event)) {
            ac_dprintf("processed: ");
            debug_record(record);
        }
    } else {
        // Not processed; enqueue in waiting_buffer
        if (!waiting_buffer_enq(record)) {
            // Buffer overflow! Clear everything
            ac_dprintf("OVERFLOW: CLEAR ALL STATES\n");
            clear_keyboard();
            waiting_buffer_clear();
            tapping_key = (keyrecord_t){0};
        }
    }

    // After processing current record, drain waiting_buffer
    for (; waiting_buffer_tail != waiting_buffer_head; waiting_buffer_tail = (waiting_buffer_tail + 1) % WAITING_BUFFER_SIZE) {
        if (process_tapping(&waiting_buffer[waiting_buffer_tail])) {
            ac_dprintf("processed: waiting_buffer[%u] =", waiting_buffer_tail);
            debug_record(waiting_buffer[waiting_buffer_tail]);
            ac_dprintf("\n\n");
        } else {
            break;  // Stop draining if a key can't be processed yet
        }
    }
}
```

### Cascading Example

**Timeline with cascading tap-holds**:
```
0ms:   SFT_T(KC_A) down
       → tapping_key = SFT_T(KC_A)
       → Undecided, waiting for TAPPING_TERM or other event

50ms:  LT(2, KC_B) down
       → Not processed (SFT_T still deciding)
       → Enqueued in waiting_buffer[0]
       → waiting_buffer_head = 1

100ms: LT(2, KC_B) released
       → Not processed (SFT_T still deciding)
       → Enqueued in waiting_buffer[1]
       → waiting_buffer_head = 2

150ms: SFT_T(KC_A) released
       → process_tapping() now processes release
       → SFT_T settles as TAP ('a')
       → tapping_key cleared

       Next iteration drains waiting_buffer:
       → waiting_buffer[0]: LT(2, KC_B) down
          → process_tapping() processes it
          → Sets tapping_key = LT(2, KC_B)
          → waiting_buffer_tail = 1

       → waiting_buffer[1]: LT(2, KC_B) released
          → process_tapping() processes it
          → LT(2, KC_B) settles as TAP ('b')
          → waiting_buffer_tail = 2

Result: "ab" (not cascading, sequential)
```

### Buffer Overflow

If more than 8 events are queued while a tap-hold is deciding:
```c
if ((waiting_buffer_head + 1) % WAITING_BUFFER_SIZE == waiting_buffer_tail) {
    // Buffer full!
    ac_dprintf("waiting_buffer_enq: Over flow.\n");
    return false;  // enqueue fails
}
```

If enqueue fails:
```c
if (!waiting_buffer_enq(record)) {
    // clear all in case of overflow.
    ac_dprintf("OVERFLOW: CLEAR ALL STATES\n");
    clear_keyboard();  // Panic: release all keys
    waiting_buffer_clear();
    tapping_key = (keyrecord_t){0};  // Reset state machine
}
```

---

## 6. QMK vs ZMK: require-prior-idle Equivalent

### ZMK's require-prior-idle-ms

In ZMK, `require-prior-idle-ms` delays the decision of whether a tap-hold key is "held" until:
1. The key has been idle (no other keys pressed) for at least `require-prior-idle-ms`, AND
2. The tap-hold key is still pressed

**Effect**: Prevents unintended holds when typing fast and rolling keys.

### QMK's Equivalent Mechanisms

QMK doesn't have a direct `require-prior-idle-ms` setting, but achieves similar goals through:

#### Option 1: PERMISSIVE_HOLD
```c
#define PERMISSIVE_HOLD
```
**Effect**: Requires a "nested" press-release before settling as hold. This effectively requires the user to complete a full keystroke (press and release) before the tap-hold decides to hold.

**This is closest to require-prior-idle**, because:
- If you're typing fast and rolling keys, PERMISSIVE_HOLD interprets it as tap
- If you pause and press-release a key, PERMISSIVE_HOLD interprets it as hold

#### Option 2: FLOW_TAP_TERM
```c
#define FLOW_TAP_TERM 150
```
**Effect**: During fast typing (keys within 150ms of each other), forces tap-hold keys to immediately settle as taps, disabling the hold behavior.

**This is effectively the opposite philosophy**: Instead of "require idle before holding," it says "disable holds during typing flow."

#### Option 3: Increase TAPPING_TERM
```c
#define TAPPING_TERM 250  // or higher
```
**Effect**: Extends the window before the key settles as a hold, giving the user more time to complete other keystrokes before the decision is made.

#### Option 4: Per-key TAPPING_TERM
```c
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LSFT_T(KC_A):  // Home row mod
            return 220;  // Longer for home row mods
        default:
            return TAPPING_TERM;
    }
}
```

#### Option 5: HOLD_ON_OTHER_KEY_PRESS with CHORDAL_HOLD
```c
#define HOLD_ON_OTHER_KEY_PRESS
#define CHORDAL_HOLD
```
**Effect**: Only settles as hold when another key is pressed **on the opposite hand**. Same-hand keys are treated as taps.

This mimics the "rolling keys" behavior that ZMK's require-prior-idle was trying to avoid.

### Fundamental Difference

**ZMK approach**: "Wait for idle time before allowing holds"
- Proactive: Prevents accidental holds during rolls
- User has to stop typing briefly for a hold to register

**QMK approaches**:
1. **PERMISSIVE_HOLD**: "Require full press-release of inner key before holding"
2. **FLOW_TAP_TERM**: "Disable holds during typing flow"
3. **CHORDAL_HOLD**: "Allow holds only for opposite-hand chords"

QMK doesn't have "require prior idle" because it solves the problem differently—through decision modes and per-hand logic.

---

## 7. FLOW_TAP_TERM Behavior Details

### When FLOW_TAP_TERM Is Checked

**File**: `action_tapping.c:279-283` (when starting a new tapping key)

```c
if (IS_NOEVENT(tapping_key.event)) {
    // ... reset state
    if (event.pressed && is_tap_record(keyp)) {
        // A tap-hold key is being pressed while no tapping_key is active

#    if defined(FLOW_TAP_TERM)
        if (flow_tap_key_if_within_term(keyp, flow_tap_prev_time)) {
            return true;  // Flow tap applied, immediately settled as tap
        }
#    endif

        // ... normal tapping start
        ac_dprintf("Tapping: Start(Press tap key).\n");
        tapping_key = *keyp;
        // ...
    }
}
```

**Second location** (from `action_tapping.c:615-623`, when releasing a tap-hold key):

```c
else if (is_tap_record(keyp)) {
    // Sequential tap can be interfered with other tap key.
#    if defined(FLOW_TAP_TERM)
    if (flow_tap_key_if_within_term(keyp, flow_tap_prev_time)) {
        tapping_key = (keyrecord_t){0};
        debug_tapping_key();
        return true;
    }
#    endif
    ac_dprintf("Tapping: Start with interfering other tap.\n");
    tapping_key = *keyp;
}
```

**Third location** (from `action_tapping.c:322-340`, when a tap-hold settles as tapped):

```c
if (tapping_key.tap.count == 0) {
    if (IS_TAPPING_RECORD(keyp) && !event.pressed) {
        // first tap!
        ac_dprintf("Tapping: First tap(0->1).\n");
        tapping_key.tap.count = 1;
        debug_tapping_key();
        process_record(&tapping_key);

#    if defined(FLOW_TAP_TERM)
        // Now that tapping_key has settled as tapped, check whether
        // Flow Tap applies to following yet-unsettled keys in the buffer
        uint16_t prev_time = tapping_key.event.time;
        for (; waiting_buffer_tail != waiting_buffer_head; waiting_buffer_tail = (waiting_buffer_tail + 1) % WAITING_BUFFER_SIZE) {
            keyrecord_t *record = &waiting_buffer[waiting_buffer_tail];
            if (!record->event.pressed) {
                break;
            }
            const int16_t next_time = record->event.time;
            if (!is_tap_record(record)) {
                process_record(record);
            } else if (!flow_tap_key_if_within_term(record, prev_time)) {
                break;  // Stop if this tap-hold doesn't qualify for flow tap
            }
            prev_time = next_time;
        }
        debug_waiting_buffer();
#    endif
    }
}
```

### The Flow Tap State Machine

**Variables** (from `action_tapping.c:123-125`):
```c
static uint16_t flow_tap_prev_keycode = KC_NO;
static uint16_t flow_tap_prev_time    = 0;
static bool     flow_tap_expired      = true;
```

**Updated by** (from `action_tapping.c:992-1030`):
```c
void flow_tap_update_last_event(keyrecord_t *record) {
    const uint16_t keycode = get_record_keycode(record, false);
    // Don't update while a tap-hold key is unsettled
    if (record->tap.count == 0 && (waiting_buffer_tail != waiting_buffer_head ||
        (tapping_key.event.pressed && tapping_key.tap.count == 0))) {
        return;
    }
    // Ignore releases of modifiers, momentaries, one-shots, etc.
    if (!record->event.pressed) {
        switch (keycode) {
            case MODIFIER_KEYCODE_RANGE:
            case QK_MOMENTARY ... QK_MOMENTARY_MAX:
            // ... many cases that don't update
            return;
        }
    }

    flow_tap_prev_keycode = keycode;
    flow_tap_prev_time    = record->event.time;
    flow_tap_expired      = false;  // Reset expiration flag
}
```

### Timeline: Complex Example

```
Config: TAPPING_TERM=200ms, FLOW_TAP_TERM=150ms

0ms:   'A' down
       → process_record('A')
       → flow_tap_update_last_event() called
       → flow_tap_prev_keycode = KC_A
       → flow_tap_prev_time = 0ms
       → flow_tap_expired = false

50ms:  'A' up
       → process_record('A')
       → flow_tap_update_last_event() called (only if settled tap)
       → flow_tap_prev_time = 50ms

80ms:  LSFT_T(KC_S) down
       → Check if no tapping_key active: YES
       → Call flow_tap_key_if_within_term(LSFT_T(KC_S), 50ms)
       → idle_time = 80 - 50 = 30ms
       → get_flow_tap_term(LSFT_T(KC_S), KC_A)
       → Both KC_S and KC_A are flow tap keys: return 150ms
       → 30ms < 150ms? YES
       → LSFT_T(KC_S) immediately settles as TAP (not waiting 200ms!)
       → result: "as"
       → flow_tap_update_last_event() called
       → flow_tap_prev_keycode = KC_S
       → flow_tap_prev_time = 80ms

90ms:  LSFT_T(KC_D) down
       → idle_time = 90 - 80 = 10ms
       → 10ms < 150ms? YES
       → LSFT_T(KC_D) immediately settles as TAP
       → result: "asd"

500ms: User pauses (no keys pressed)

510ms: LSFT_T(KC_F) down
       → idle_time = 510 - (time of last key release)
       → idle_time > 500ms
       → flow_tap_expired becomes true (from line 1034)
       → flow_tap_key_if_within_term() returns false
       → LSFT_T(KC_F) enters normal tapping mode
       → Waits full 200ms for TAPPING_TERM
       → If held, becomes Shift
```

### Key Insights on FLOW_TAP_TERM

1. **Not a "term" you wait for** — it's a retrospective idle-time check
2. **Disables hold behavior** during fast typing (< 500ms idle time)
3. **Immediately forces tap** if the previous key was recent enough
4. **Capped at 500ms**: Even if you configure FLOW_TAP_TERM=1000ms, it caps at 500ms
5. **Only for specific keys**: Default is alphas + space; can be customized
6. **Can be disabled per-key/per-combo** via `get_flow_tap_term()` returning 0
7. **Tracks the "flow" of typing**: Each keystroke resets the timeout for the next

---

## 8. Key Press Rollover During Tap-Hold Window

### What Happens to Keys Pressed During TAPPING_TERM

**File**: `action_tapping.c:305-502` (the main flow)

When a second key is pressed while a tap-hold key is undecided:

```c
if (event.pressed) {
    tapping_key.tap.interrupted = true;  // Mark as interrupted (line 451)

#    if defined(CHORDAL_HOLD)
    if (is_mt_or_lt(tapping_keycode) && !get_chordal_hold(...)) {
        // Same-hand chord: settle as tap immediately
        // (covered in section 4)
    } else
#    endif
    if (TAP_GET_HOLD_ON_OTHER_KEY_PRESS) {
        // HOLD_ON_OTHER_KEY_PRESS enabled: settle as hold
        ac_dprintf("Tapping: End. No tap. Interfered by pressed key\n");
        process_record(&tapping_key);
        tapping_key = (keyrecord_t){0};
        // The incoming key is NOT processed yet; it waits
    } else {
        // Default mode: buffer the key, don't process it yet
        return false;  // Don't process, enqueue in waiting_buffer
    }
}
```

### Default Mode Behavior (Most Common)

**Keys pressed during TAPPING_TERM are BUFFERED**:

1. Incoming key is **not immediately processed**
2. It's added to `waiting_buffer[]`
3. Processing is **delayed** until the tap-hold key is decided
4. Once decided, the buffered key is processed in order

**Example**:
```
0ms:   SFT_T(KC_A) down
       → tapping_key = SFT_T(KC_A), tap.count = 0

50ms:  KC_B down
       → SFT_T(KC_A) still undecided
       → KC_B NOT processed
       → Enqueued: waiting_buffer[0] = KC_B down
       → Return false from process_tapping() (not processed)

80ms:  KC_B up
       → SFT_T(KC_A) still undecided
       → KC_B up NOT processed
       → Enqueued: waiting_buffer[1] = KC_B up

150ms: SFT_T(KC_A) up
       → Within TAPPING_TERM
       → Settles as TAP
       → process_record(SFT_T) sends 'a'

       Waiting buffer drained:
       → waiting_buffer[0]: KC_B down is now processed
       → waiting_buffer[1]: KC_B up is now processed

       Result sent to host: 'a', then 'b' (never shifted)
```

### HOLD_ON_OTHER_KEY_PRESS Mode

**When another key is pressed, the tap-hold immediately settles as HOLD**:

```c
if (TAP_GET_HOLD_ON_OTHER_KEY_PRESS) {
    ac_dprintf("Tapping: End. No tap. Interfered by pressed key\n");
    process_record(&tapping_key);  // Settle as HOLD

#    if defined(CHORDAL_HOLD)
    if (waiting_buffer_tail != waiting_buffer_head && is_tap_record(&waiting_buffer[waiting_buffer_tail])) {
        tapping_key = waiting_buffer[waiting_buffer_tail];
        waiting_buffer_tail = (waiting_buffer_tail + 1) % WAITING_BUFFER_SIZE;
        debug_waiting_buffer();
    } else
#    endif
    {
        tapping_key = (keyrecord_t){0};
    }
}
```

**Timeline with HOLD_ON_OTHER_KEY_PRESS**:
```
0ms:   SFT_T(KC_A) down
       → tapping_key = SFT_T(KC_A)

50ms:  KC_B down
       → SFT_T(KC_A) still within tapping term
       → HOLD_ON_OTHER_KEY_PRESS enabled
       → Immediately settle SFT_T(KC_A) as HOLD
       → process_record(SFT_T) registers Shift key down
       → KC_B down NOT yet processed (still in waiting_buffer)

80ms:  KC_B up
       → Enqueued in waiting_buffer

150ms: SFT_T(KC_A) up
       → Shift key up

       Waiting buffer drained:
       → KC_B down processed (while Shift is active) → 'B'
       → KC_B up processed

Result: 'B' (shifted)
```

### PERMISSIVE_HOLD Mode

**Behavior**: Similar to default, but if the interrupting key is **press-released** (completed) before TAPPING_TERM, settles as HOLD:

```c
else if (
    !event.pressed && waiting_buffer_typed(event) &&
    (
        TAP_GET_PERMISSIVE_HOLD ||
        TAP_GET_RETRO_TAPPING(keyp)
    )
) {
    // The interrupting key was pressed and released (nested tap)
    ac_dprintf("Tapping: End. No tap. Interfered by typing key\n");
    process_record(&tapping_key);  // Settle as HOLD
    // ...
}
```

**Timeline with PERMISSIVE_HOLD**:
```
0ms:   SFT_T(KC_A) down
50ms:  KC_B down
60ms:  KC_B released        ← Nested press-release
150ms: SFT_T(KC_A) released
       → At KC_B release, process_tapping() called with KC_B up
       → PERMISSIVE_HOLD enabled + nested tap detected
       → Settle SFT_T(KC_A) as HOLD
       → process_record() sends Shift
       → KC_B was already processed while undecided

Result: 'B' (shifted, but KC_B was processed when sent, not shifted)
```

Wait, that doesn't sound right. Let me reconsider...

Actually, in PERMISSIVE_HOLD, when the nested tap is detected (at KC_B release), the tap-hold is settled as HOLD, but KC_B has already been in the waiting_buffer. Looking at line 380:

```c
else if (
    !event.pressed && waiting_buffer_typed(event) &&
    (
        TAP_GET_PERMISSIVE_HOLD ||
        TAP_GET_RETRO_TAPPING(keyp)
    )
) {
    ac_dprintf("Tapping: End. No tap. Interfered by typing key\n");
    process_record(&tapping_key);
    // ...
    tapping_key = (keyrecord_t){0};
    debug_tapping_key();
    // enqueue
    return false;  // Requeue the current event (KC_B up)
}
```

The tap-hold is settled as HOLD (tap.count = 0 means hold), then the current event (KC_B release) is requeued (`return false`), and the waiting_buffer is processed. So KC_B down was buffered, then at KC_B up, SFT_T is settled as HOLD, then KC_B down is processed (with Shift active), then KC_B up is processed.

So the result would indeed be 'B' (shifted).

### Summary: Rollover Behavior by Mode

| Mode | When Second Key Pressed | Result |
|------|------------------------|--------|
| **Default** | Buffered, processed after decision | Key not shifted; press delayed |
| **PERMISSIVE_HOLD** | Buffered, but if press+release before TAPPING_TERM, becomes HOLD | Key can be shifted if nested |
| **HOLD_ON_OTHER_KEY_PRESS** | Immediately settles as HOLD | Key is shifted |
| **FLOW_TAP_TERM** | If within flow term, immediately settles as TAP | Key definitely not shifted |

---

## Summary: QMK vs ZMK Timing Fundamentals

### QMK's Philosophy
- **One undecided key at a time**, others buffered
- **Decision modes** change when the tap-hold decision is made (not just what the decision is)
- **FLOW_TAP_TERM** disables holds during typing flow
- **CHORDAL_HOLD** adds spatial logic (same vs opposite hand)
- **No require-prior-idle** equivalent; instead, use PERMISSIVE_HOLD or increase TAPPING_TERM

### ZMK's Philosophy (for comparison)
- **Parallel decision-making** for multiple tap-holds
- **require-prior-idle-ms** prevents holds until typing has fully stopped
- **Positional (on-press/on-release)** and **flavor (tap-preferred/hold-preferred)** control behavior

### Why QMK's TAPPING_TERM Adjustments Feel Different

1. **Different timing windows**: QMK's TAPPING_TERM doesn't just change the timeout, it changes the entire state machine's behavior
2. **Decision mode coupling**: The actual behavior depends on which mode is enabled, not just the TAPPING_TERM value
3. **Buffering overhead**: Keys pressed during TAPPING_TERM are delayed until the decision is made
4. **No per-hand idle tracking**: QMK checks handedness at decision time, not continuously while typing

---

## Files Referenced

- `/home/naughty/qmk/qmk_firmware/quantum/action_tapping.h` — Header with configuration macros
- `/home/naughty/qmk/qmk_firmware/quantum/action_tapping.c` — Core implementation
- `/home/naughty/qmk/qmk_firmware/docs/tap_hold.md` — Official documentation
- `/home/naughty/qmk/qmk_firmware/docs/mod_tap.md` — Mod-tap specific documentation
