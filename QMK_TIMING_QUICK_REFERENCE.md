# QMK Tap-Hold Timing - Quick Reference Guide

## Three Core Timing Constants

```c
#define TAPPING_TERM 200              // Decision timeout (ms)
#define QUICK_TAP_TERM TAPPING_TERM   // Sequential tap window (ms)
#define FLOW_TAP_TERM 150             // Fast typing override (ms)
```

### TAPPING_TERM
- **When**: Key pressed → timeout expires OR decision triggered
- **Tap if**: Released before timeout AND no interrupt (depending on mode)
- **Hold if**: Held past timeout OR interrupt condition met (depending on mode)

### QUICK_TAP_TERM
- **When**: After a tap-hold settles as TAP, user presses it again
- **Effect**: If within QUICK_TAP_TERM → sequential tap (counts, auto-repeats)
- **Effect**: If after QUICK_TAP_TERM → new press (settles as hold immediately)

### FLOW_TAP_TERM
- **When**: User presses tap-hold within FLOW_TAP_TERM of previous key
- **Effect**: Immediately settles current key as TAP (no waiting for TAPPING_TERM)
- **Not a timeout you wait for** — it's a "was the previous key recent?"check
- **Max 500ms**: Even if configured higher, caps at 500ms idle time

---

## Three Decision Modes (When/How Tap-Hold Becomes Hold)

### Mode 1: Default
```c
// No config needed; default behavior
```
**Decision triggers**:
- Timeout: Key held > TAPPING_TERM
- Other keys are BUFFERED, don't affect decision

**Other keys during TAPPING_TERM**: Delayed, processed after decision

**Use case**: Simplicity, predictability

### Mode 2: PERMISSIVE_HOLD
```c
#define PERMISSIVE_HOLD
```
**Decision triggers**:
- Timeout: Key held > TAPPING_TERM, OR
- Interrupt: Other key **pressed AND released** (nested) before TAPPING_TERM

**Other keys during TAPPING_TERM**:
- If press+release before timeout → triggers HOLD
- If still held when timeout → doesn't trigger HOLD (rolling press)

**Use case**: Fast typists; prevent accidental taps when nesting keys

### Mode 3: HOLD_ON_OTHER_KEY_PRESS
```c
#define HOLD_ON_OTHER_KEY_PRESS
```
**Decision triggers**:
- Timeout: Key held > TAPPING_TERM, OR
- Interrupt: Any other key **pressed** before TAPPING_TERM

**Other keys during TAPPING_TERM**:
- Any key press triggers HOLD immediately
- No distinction between rolling and nesting

**Use case**: Fast typists; want modifier active as soon as another key is touched

---

## CHORDAL_HOLD: Spatial Logic

```c
#define CHORDAL_HOLD
#define HOLD_ON_OTHER_KEY_PRESS  // Usually paired with this
```

**Logic**: Checks if two keys are on same/opposite hands

- **Same hand** → Settles as TAP immediately
- **Opposite hand** → Respects TAPPING_TERM + decision mode
- **Exempt key (*)** → Always allows hold

**Result**:
- Prevents accidental Shift in fast same-hand rolling (e.g., `'a'` then `'c'` quickly → `"ac"`, not `"C"`)
- Allows hold for opposite-hand combos (e.g., left-hand mod + right-hand key)

**Define handedness**:
```c
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM = LAYOUT(
    'L', 'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R', 'R',
    // ...
);

// Or via function:
char chordal_hold_handedness(keypos_t key) {
    return key.col < MATRIX_COLS / 2 ? 'L' : 'R';
}
```

---

## FLOW_TAP_TERM Behavior

**Configuration**:
```c
#define FLOW_TAP_TERM 150  // milliseconds

bool is_flow_tap_key(uint16_t keycode) {  // Which keys enable flow tap?
    switch (get_tap_keycode(keycode)) {
        case KC_A ... KC_Z:
        case KC_SPC:
        case KC_DOT:
        case KC_COMM:
        // ... (alphas + space by default)
            return true;
    }
    return false;
}

uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record, uint16_t prev_keycode) {
    // Return 0 to disable for this key, or ms value
    if (is_flow_tap_key(keycode) && is_flow_tap_key(prev_keycode)) {
        return FLOW_TAP_TERM;  // Both are flow keys
    }
    return 0;  // Disable flow tap for this combo
}
```

**How it works**:
1. Every key press updates `flow_tap_prev_time` and `flow_tap_prev_keycode`
2. When new tap-hold key pressed, check: `idle_time = current_time - flow_tap_prev_time`
3. If `idle_time < FLOW_TAP_TERM` → immediately settle as TAP
4. If `idle_time >= 500ms` → flow tap expired, normal TAPPING_TERM applies

**Effect**: Disables hold behavior during fast typing, provides tap immediately

---

## Per-Key Configuration

### Per-Key Tapping Term
```c
#define TAPPING_TERM_PER_KEY

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case SFT_T(KC_SPC):
            return TAPPING_TERM + 100;  // Space shift: longer
        case LT(1, KC_GRV):
            return 100;  // Quick layer tap: shorter
        default:
            return TAPPING_TERM;
    }
}
```

### Per-Key Decision Mode
```c
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY

bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(1, KC_BSPC):
            return true;  // This key uses mode 3
        case LSFT_T(KC_A):
            return false;  // This key uses mode 1 (default)
        default:
            return false;
    }
}
```

### Per-Key QUICK_TAP_TERM
```c
#define QUICK_TAP_TERM_PER_KEY

uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LSFT_T(KC_SPC):
            return QUICK_TAP_TERM - 50;  // Shorter auto-repeat window
        default:
            return QUICK_TAP_TERM;
    }
}
```

---

## Cascading Tap-Holds: How QMK Prevents It

**The Problem**: What if user presses SFT_T(A), then LT(2, B) before SFT_T(A) is decided?

**QMK's Solution: Waiting Buffer**
- Only ONE tap-hold can be actively deciding at a time (`tapping_key`)
- Other tap-holds are queued in `waiting_buffer` (size: 8 events max)
- Once the active key is decided, buffered keys are processed in order

**Timeline**:
```
0ms:   SFT_T(A) down       → tapping_key = SFT_T(A), undecided
50ms:  LT(2, B) down       → buffered in waiting_buffer[0]
150ms: SFT_T(A) released
       → SFT_T(A) settles as TAP
       → Drain buffer: process LT(2, B) down
       → LT(2, B) becomes new tapping_key
```

**Buffer overflow** (> 8 events while deciding): Clears keyboard and resets state machine

---

## Decision Timeline Comparison

### Default Mode
```
    | TAPPING_TERM
    ↓
0---+---50(KeyB down)---100(KeyB up)---200---release
    | [buffered]         [buffered]
Outcome: TAP + 'b'
```

### PERMISSIVE_HOLD
```
    | TAPPING_TERM
    ↓
0---+---50(KeyB down)---100(KeyB up)---200---release
    |                     ↑ nested detected → HOLD
Outcome: HOLD (shift)
```

### HOLD_ON_OTHER_KEY_PRESS
```
    | TAPPING_TERM
    ↓
0---+---50(KeyB down)---100(KeyB up)---200---release
    |   ↑ any press detected → HOLD
Outcome: HOLD (shift)
```

### FLOW_TAP_TERM
```
Previous key at: 50ms
Current tap-hold at: 80ms
    | FLOW_TAP_TERM (150ms)
    ↓
0---50(previous)---80(this)---200
    |               ↑ idle=30ms < 150ms → TAP now
Outcome: TAP immediately (not waiting 200ms)
```

---

## Common Configuration Patterns

### Home Row Mods (Best Practice)
```c
#define TAPPING_TERM 200
#define TAPPING_TERM_PER_KEY
#define PERMISSIVE_HOLD_PER_KEY
#define CHORDAL_HOLD
#define FLOW_TAP_TERM 150

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Extend tapping term for home row mods
        case LSFT_T(KC_A):
        case LCTL_T(KC_S):
        case LALT_T(KC_D):
        case LGUI_T(KC_F):
        case RALT_T(KC_J):
        case RGUI_T(KC_K):
        case RCTL_T(KC_L):
        case RSFT_T(KC_SCLN):
            return TAPPING_TERM + 50;
        default:
            return TAPPING_TERM;
    }
}

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    // Enable only for home row mods
    return record->event.key.row == HOME_ROW;
}
```

### Fast Typist (No Hold-on-Other)
```c
#define TAPPING_TERM 220
#define PERMISSIVE_HOLD
#define CHORDAL_HOLD
#define FLOW_TAP_TERM 120
// No HOLD_ON_OTHER_KEY_PRESS
```

### Gaming (Prioritize Hold)
```c
#define TAPPING_TERM 100
#define HOLD_ON_OTHER_KEY_PRESS
// No FLOW_TAP_TERM (want mods responsive)
// No PERMISSIVE_HOLD (would conflict)
```

---

## Debugging: Understanding Your Timing

Enable `ACTION_DEBUG` to see what's happening:

```c
#define ACTION_DEBUG  // In config.h
```

Output in CONSOLE:
```
"Tapping: Start(Press tap key)."              → Key entered tapping state
"Tapping: First tap(0->1)."                   → Key settled as TAP
"Tapping: End. Timeout. Not tap(0):"          → TAPPING_TERM expired
"Tapping: End. No tap. Interfered by pressed key"  → HOLD_ON_OTHER_KEY_PRESS triggered
"Tapping: End. No tap. Interfered by typing key"   → PERMISSIVE_HOLD triggered
```

---

## Key Differences from ZMK

| Aspect | QMK | ZMK |
|--------|-----|-----|
| **Multiple undecided keys** | Buffered (one at a time) | Can be parallel |
| **Require idle** | No direct equivalent; use PERMISSIVE_HOLD + CHORDAL_HOLD | `require-prior-idle-ms` |
| **Fast typing override** | FLOW_TAP_TERM (disables hold) | Not applicable |
| **Spatial logic** | CHORDAL_HOLD (same vs opposite hand) | No equivalent |
| **Decision modes** | 3 modes (default, permissive, hold-on-other) | Positional + flavor |
| **Configuration complexity** | High (many interdependent options) | Medium (more modular) |

---

## Troubleshooting

### "Keys are delayed"
- **Cause**: Waiting for tap-hold decision before processing buffered keys
- **Solution**:
  - Increase TAPPING_TERM? (No, makes it worse)
  - Enable FLOW_TAP_TERM? (Reduces delay during typing)
  - Use HOLD_ON_OTHER_KEY_PRESS? (Settles faster)

### "Accidental mods in fast typing"
- **Cause**: Rolling keys; tap-hold settles as hold
- **Solution**:
  - Enable CHORDAL_HOLD (prevents same-hand holds)
  - Enable FLOW_TAP_TERM (disables hold during typing flow)
  - Increase TAPPING_TERM (give more time to type)

### "Can't trigger modifier hold"
- **Cause**: PERMISSIVE_HOLD or FLOW_TAP_TERM preventing it
- **Solution**:
  - Disable FLOW_TAP_TERM for that key
  - Or: Pause briefly before pressing modifier key
  - Or: Use non-alpha keys after modifier (not covered by FLOW_TAP_TERM)

### "Sequential taps not working (double-tap)"
- **Cause**: QUICK_TAP_TERM too short
- **Solution**: Increase QUICK_TAP_TERM
- **Note**: QUICK_TAP_TERM defaults to TAPPING_TERM; max is TAPPING_TERM

---
