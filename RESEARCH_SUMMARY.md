# QMK Tap-Hold Timing Research - Summary

## Documents Created

1. **QMK_TAP_HOLD_TIMING_RESEARCH.md** (Main Document)
   - Comprehensive breakdown of all timing mechanisms
   - Detailed code analysis with file references
   - Explanation of each system component
   - How cascading is prevented
   - Complete FLOW_TAP_TERM mechanics

2. **QMK_TIMING_QUICK_REFERENCE.md** (Cheat Sheet)
   - Three core timing constants (TAPPING_TERM, QUICK_TAP_TERM, FLOW_TAP_TERM)
   - Three decision modes at a glance
   - CHORDAL_HOLD mechanics
   - Per-key configuration templates
   - Common configuration patterns
   - Troubleshooting guide

3. **QMK_vs_ZMK_TIMING_COMPARISON.md** (Architecture Analysis)
   - QMK's sequential decision model vs ZMK's parallel model
   - Why FLOW_TAP_TERM was needed
   - require-prior-idle-ms equivalents in QMK
   - How each system prevents cascading
   - Configuration complexity comparison
   - Recommended QMK settings for ZMK users

---

## Key Findings

### 1. TAPPING_TERM, QUICK_TAP_TERM, FLOW_TAP_TERM Interaction

**TAPPING_TERM (200ms default)**
- Primary decision timeout
- Not the only factor: affected by decision mode and interrupts
- Checked via `WITHIN_TAPPING_TERM(e)` macro

**QUICK_TAP_TERM (defaults to TAPPING_TERM)**
- Controls auto-repeat and sequential tap behavior
- When user presses the same key again after tapping it
- If within QUICK_TAP_TERM → sequential tap (increments tap.count)
- If after QUICK_TAP_TERM → new press (starts new tapping_key)
- Setting to 0 disables auto-repeat

**FLOW_TAP_TERM (optional, e.g., 150ms)**
- **Not** a timeout you wait for
- **Is** a retroactive idle-time check
- When new tap-hold pressed, asks: "Did previous key come within FLOW_TAP_TERM of this press?"
- If YES → immediately settles current key as TAP (bypasses TAPPING_TERM wait)
- If NO → waits for full TAPPING_TERM
- Capped at 500ms regardless of configuration
- Disables hold behavior during fast typing

**Interaction**: FLOW_TAP_TERM can override TAPPING_TERM by forcing a TAP decision before the timeout expires. This is QMK's speed optimization.

---

### 2. Per-Key Tapping Term Behavior

**How it works**: Enable `#define TAPPING_TERM_PER_KEY` and implement:
```c
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    // Return custom timeout for each key
}
```

**Important**: Each key's timeout is evaluated independently. When multiple tap-holds queue up:
- Only the first tap-hold key actively uses its TAPPING_TERM
- Others are buffered and processed sequentially
- Each becomes active once the previous is decided

**Override behavior**: YES, per-key values override TAPPING_TERM globally for that specific key. Other keys still use TAPPING_TERM.

**Key insight**: The value returned is used immediately in `WITHIN_TAPPING_TERM(e)` checks, so it effectively overrides the global constant for timing calculations.

---

### 3. Hold-Tap Flavors (Decision Modes)

QMK doesn't call them "flavors"—it calls them **decision modes**. Three exist, each answering "when does this key settle as HOLD?":

**Mode 1: Default (No extra config)**
- Hold when: Key held > TAPPING_TERM, OR timeout expires
- Interrupt behavior: Other keys are buffered, don't affect decision
- Other keys during tapping term: Delayed, processed after decision
- Use case: Simplicity

**Mode 2: PERMISSIVE_HOLD** (Enabled: `#define PERMISSIVE_HOLD`)
- Hold when: Key held > TAPPING_TERM, OR other key is nested (press+release before timeout)
- Interrupt behavior: Distinguishes nested (press+release) from rolling (still held)
- Other keys during tapping term: If completed (press+release), triggers HOLD
- Use case: Prevent taps during nested key sequences

**Mode 3: HOLD_ON_OTHER_KEY_PRESS** (Enabled: `#define HOLD_ON_OTHER_KEY_PRESS`)
- Hold when: Key held > TAPPING_TERM, OR any other key pressed before timeout
- Interrupt behavior: Immediately settles as hold on any key press
- Other keys during tapping term: ANY press triggers HOLD
- Use case: Fastest feedback for modifiers

**Per-key variant**: `#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY` lets you mix modes per key.

**Key difference from ZMK**: QMK modes change WHEN the decision is made (reactive to interrupts), not just WHAT the decision is.

---

### 4. CHORDAL_HOLD Mechanics

**What it does**: Adds a spatial handedness check during tap-hold decisions.

**Logic**: When another key is pressed during tapping term, check if they're on the same or opposite hands.
- **Same hand** → Immediately settle as TAP (prevents accidental mods on rolls)
- **Opposite hand** → Respects TAPPING_TERM + decision mode (allows intentional chords)
- **Exempt key (*)** → Always allows hold

**Important**: CHORDAL_HOLD doesn't change the TAPPING_TERM value itself. It adds a spatial check BEFORE the timeout check.

**Effect on timing**:
- Same-hand chords settle faster (no waiting for TAPPING_TERM)
- Opposite-hand chords still wait for TAPPING_TERM
- Combined with HOLD_ON_OTHER_KEY_PRESS: opposite-hand chords trigger hold immediately

**Handedness definition**: Specify via layout array or function:
```c
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] = LAYOUT(...);
// or
char chordal_hold_handedness(keypos_t key) { return ...; }
```

**Key insight**: CHORDAL_HOLD is a spatial filter that prevents same-hand accidental holds while allowing opposite-hand intentional ones.

---

### 5. Cascading Tap-Hold Prevention

**The problem**: What happens when you press one tap-hold key, then another before the first is decided?

**QMK's solution**: **Waiting buffer** with single active tapping_key

**Implementation**:
- Only ONE `tapping_key` is actively deciding at a time
- Additional tap-holds are stored in `waiting_buffer[8]` (8-event capacity)
- Once active key is decided, buffered keys are processed in FIFO order
- Each buffered key becomes the new `tapping_key` when processed

**Timeline example**:
```
0ms:   SFT_T(A) down → tapping_key = SFT_T(A)
50ms:  LT(2, B) down → buffered in waiting_buffer[0]
150ms: SFT_T(A) released → settles as TAP
       Drain buffer: LT(2, B) down → tapping_key = LT(2, B)
```

**Buffer overflow** (> 8 events while deciding): Clears keyboard and resets state machine (panic recovery).

**Consequence**: Keys after the first tap-hold are delayed by the first's decision time. This is why FLOW_TAP_TERM helps—it shortens the decision window.

**Difference from ZMK**: ZMK decides multiple tap-holds in parallel. QMK queues them sequentially.

---

### 6. Require-Prior-Idle Equivalent in QMK

**ZMK's require-prior-idle-ms**: Prevents hold unless X milliseconds of idle time has passed

**QMK equivalents** (no single setting, but combinations work):

1. **PERMISSIVE_HOLD + CHORDAL_HOLD** (Closest match)
   - PERMISSIVE_HOLD: Requires nested press-release (completed keystroke) to trigger hold
   - CHORDAL_HOLD: Prevents same-hand holds (rolling keys stay as taps)
   - Result: Similar to require-prior-idle for home row mods

2. **FLOW_TAP_TERM** (Opposite philosophy)
   - Disables hold during typing flow (< 500ms idle)
   - Allows hold during pauses (> 500ms idle)
   - Result: Auto-adapt to typing speed

3. **Increase TAPPING_TERM** (Brute force)
   - Extends decision window, giving more time for idle
   - Simple but not elegant

**Key difference**: QMK doesn't have active idle tracking like ZMK. Instead, it uses:
- Spatial logic (CHORDAL_HOLD: same vs opposite hand)
- Nested vs rolling detection (PERMISSIVE_HOLD)
- Speed detection (FLOW_TAP_TERM: fast vs pause)

---

### 7. FLOW_TAP_TERM Behavior Details

**When it's checked**:
- Line 280: When starting new tapping_key (no current tap-hold)
- Line 618: When pressing new tap-hold after previous one released
- Line 322-340: When previous tap-hold settles as TAP, check buffered keys

**How it works**:
1. Maintains `flow_tap_prev_time` and `flow_tap_prev_keycode`
2. On new tap-hold press, calculates: `idle_time = current_time - flow_tap_prev_time`
3. If `idle_time < FLOW_TAP_TERM` → immediately settle as TAP (tap.count = 1)
4. If `idle_time >= 500ms` → flow_tap_expired flag prevents further flow tap

**Constraints**:
- Only applies to "flow tap keys" (alphas + space by default)
- Can be customized via `is_flow_tap_key()` and `get_flow_tap_term()`
- Capped at 500ms maximum (line 1042)

**Effect**: Provides immediate tap response during typing (< 150ms idle), but allows hold after pause (> 500ms idle).

**Key insight**: FLOW_TAP_TERM is ZMK-like smart timing. It asks "is the user typing fast?" and adapts behavior accordingly.

---

### 8. Key Press Rollover During Tap-Hold Window

**What happens when you press another key during TAPPING_TERM**:

**Default mode**: Key is BUFFERED
- Not processed immediately
- Queued in waiting_buffer
- Processed after tap-hold decision
- Result: Other key delayed

**HOLD_ON_OTHER_KEY_PRESS**: Tap-hold immediately settles as HOLD
- Other key press triggers hold instantly
- Other key processed AFTER hold settles
- Result: Other key is shifted/modified

**PERMISSIVE_HOLD**: Tap-hold settles as HOLD if key is nested (press+release)
- If other key is still held when TAPPING_TERM expires → tap
- If other key is released before tap-hold releases → hold
- Result: Depends on release order (nested vs rolling)

**FLOW_TAP_TERM**: Tap-hold immediately settles as TAP
- If idle time < FLOW_TAP_TERM, no waiting
- Overrides other decision modes
- Result: Key definitely not shifted

**Code reference**: `action_tapping.c:305-502` shows the full decision tree.

---

## Comparison Table: QMK vs ZMK

| Aspect | QMK | ZMK |
|--------|-----|-----|
| **Cascading model** | Sequential (waiting_buffer) | Parallel (independent timers) |
| **Timing constants** | TAPPING_TERM, QUICK_TAP_TERM, FLOW_TAP_TERM | tapping-term-ms, quick-tap-ms, require-prior-idle-ms |
| **Decision modes** | Default, PERMISSIVE_HOLD, HOLD_ON_OTHER_KEY_PRESS | balanced, tap-preferred, hold-preferred |
| **Spatial logic** | CHORDAL_HOLD (optional) | None |
| **Speed adaptation** | FLOW_TAP_TERM (optional) | require-prior-idle-ms (built-in) |
| **Configuration** | Many interdependent options | Fewer, more modular |
| **Latency** | May delay keys during decision | Generally faster due to parallelism |
| **Memory** | Light (8-event buffer) | More (per-key state) |
| **CPU** | Minimal | More decision logic |

---

## Why Your Timing Feels Different

**Root cause**: QMK queues tap-holds sequentially; ZMK decides them in parallel.

**Timeline comparison** (3 tap-holds in 100ms):
- **QMK**: `SFT_T(A) waits` → (decides at 150ms) → `LT(2, B) waits` → (decides) → `KC_C` processed
- **ZMK**: `SFT_T(A), LT(2, B), KC_C` all decide in parallel → all process quickly

**Why adjusting TAPPING_TERM alone doesn't fix it**: The value is correct, but the architecture is different. You need to enable appropriate decision modes + FLOW_TAP_TERM to match ZMK's feel.

**Recommended for ZMK users**:
```c
#define TAPPING_TERM 200
#define PERMISSIVE_HOLD
#define CHORDAL_HOLD
#define FLOW_TAP_TERM 150
```

This combination mimics ZMK behavior:
- PERMISSIVE_HOLD: Wait for nested keystroke (like require-prior-idle)
- CHORDAL_HOLD: Allow opposite-hand chords (prevents rolls)
- FLOW_TAP_TERM: Fast typing adaptation (like require-prior-idle working in reverse)

---

## Code References

**Main files**:
- `/home/naughty/qmk/qmk_firmware/quantum/action_tapping.h` — Configuration macros and declarations
- `/home/naughty/qmk/qmk_firmware/quantum/action_tapping.c` — Full implementation (1100+ lines)

**Key functions**:
- `action_tapping_process()` — Entry point, handles waiting_buffer
- `process_tapping()` — Main state machine (lines 253-651)
- `flow_tap_key_if_within_term()` — FLOW_TAP_TERM logic (lines 1032-1055)
- `waiting_buffer_enq()` — Queue management (lines 657-673)

**Documentation**:
- `/home/naughty/qmk/qmk_firmware/docs/tap_hold.md` — Official tap-hold guide
- `/home/naughty/qmk/qmk_firmware/docs/mod_tap.md` — Mod-tap specifics

---

## Final Insight

QMK's tap-hold system is **powerful but complex** because it accumulated features over years. Each option (PERMISSIVE_HOLD, FLOW_TAP_TERM, CHORDAL_HOLD, SPECULATIVE_HOLD, RETRO_TAPPING) solves a specific problem.

The key to understanding it is recognizing:
1. **One active tapping_key** at a time (sequential model)
2. **Three decision modes** (change when decision is made)
3. **Four types of configuration** (global, per-key, decision mode, special)
4. **Waiting buffer** (prevents cascading but adds latency)

If you approach it as "adjust TAPPING_TERM and use the defaults," it will feel laggy compared to ZMK. But with the right combination of options enabled, QMK can match (or exceed) ZMK's responsiveness.

---
