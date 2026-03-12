---
name: qmk-add-combo
description: Guided walkthrough for adding a new combo to the QMK userspace
disable-model-invocation: true
allowed-tools: Read, Grep, Glob
---

Guide the user through adding a new combo. Read the current combo state first, then walk through each step.

**IMPORTANT**: QMK combos work differently from ZMK. In this userspace:
- `COMBO_ONLY_FROM_LAYER 0` means combos match by BASE layer keycodes
- Home row mod positions MUST use `HM_*` macros in key arrays (not bare `KC_*`)
- No per-board position overrides needed (unlike ZMK)
- Layer filtering is done in `combo_should_trigger()`, not in the combo definition

All combo code lives in one file: `users/naughtyusername/combos.h`

## Step 1: Gather Requirements

Ask the user:
1. **Trigger keys** — which keys? (by letter, e.g. "W+E" or "J+K+L")
2. **Binding** — what does it do?
   - Simple keycode: `KC_ESC`, `KC_TAB`, etc. → use `COMBO()`
   - Multi-key sequence: `()`, `!=`, etc. → use `COMBO_ACTION()` + `process_combo_event()` case
   - One-shot mod: `OSM(MOD_LSFT)` → use `COMBO()`
   - Layer switch: `TO(_GAMING)` → use `COMBO()`
3. **Active layers** — which layers should it fire on?
4. **Timeout tier**:
   - COMBO_FAST (18ms) — one-shot mods, top row utilities. Tight to prevent misfires
   - COMBO_MED (30ms) — standard combos. Default if not specified in `get_combo_term()`
   - COMBO_SLOW (50ms) — three-finger layer switches, deliberate actions

## Step 2: Conflict Check

Read `combos.h` and check:
- Do any existing combos use the same keys on overlapping layers?
- Same keys CAN have different combos if `combo_should_trigger()` filters them to non-overlapping layers
- **If sharing keys with an existing combo, BOTH must have identical timing in `get_combo_term()`** — different timing on same keys causes swallowed keypresses

## Step 3: Walk Through Each Required Change (all in `combos.h`)

### 3a. Add Enum Entry

Add to `enum combo_names` before `COMBO_LENGTH`:
```c
COMBO_XX_DESCRIPTIVE, // KEY1 + KEY2 = Output
```

### 3b. Add PROGMEM Key Array

```c
const uint16_t PROGMEM combo_xx[] = {KEY1, KEY2, COMBO_END};
```

**CRITICAL**: Use `HM_*` for home row positions:
- A → `HM_A`, S → `HM_S`, D → `HM_D`, F → `HM_F`
- J → `HM_J`, K → `HM_K`, L → `HM_L`, ; → `HM_SCLN`
- Z → `LT(_SYS, KC_Z)`, / → `LT(_SYS, KC_SLSH)` (these are layer-tap on BASE)
- All other keys use bare `KC_*`

### 3c. Add key_combos[] Entry

For simple keycodes:
```c
[COMBO_XX_DESCRIPTIVE] = COMBO(combo_xx, KC_OUTPUT),
```

For multi-key sequences (needs `process_combo_event`):
```c
[COMBO_XX_DESCRIPTIVE] = COMBO_ACTION(combo_xx),
```

### 3d. Add process_combo_event() Case (if COMBO_ACTION)

```c
case COMBO_XX_DESCRIPTIVE:
    SEND_STRING("output text" SS_TAP(X_LEFT));  // or whatever
    break;
```

### 3e. Add get_combo_term() Case (if not MED)

If FAST or SLOW, add to the appropriate switch group:
```c
case COMBO_XX_DESCRIPTIVE:
    return COMBO_FAST;  // or COMBO_SLOW
```

If MED (30ms), no entry needed — it's the default.

### 3f. Add combo_should_trigger() Case

```c
case COMBO_XX_DESCRIPTIVE:
    return layer_state_is(_BASE) || layer_state_is(_VIM);
```

Group with existing combos that have the same layer filter.

### 3g. (Optional) Add get_combo_must_hold() or get_combo_must_tap()

- `must_hold`: For combos that should only fire when held (prevents accidental activation)
- `must_tap`: Auto-detected for combos containing mod-tap keys, but can be overridden

## Step 4: Update the Combo Index Table

Add an entry to the ASCII reference table at the top of `combos.h`:
```
 *  │ COMBO_XX_DESCRIPTIVE  │ KEYS │ Output     │ Timing  │ Layers           │
```

## Step 5: Summary Checklist

All changes are in `users/naughtyusername/combos.h`:
- [ ] Enum entry added before COMBO_LENGTH
- [ ] PROGMEM key array with correct keycodes (HM_* for home row)
- [ ] key_combos[] entry (COMBO or COMBO_ACTION)
- [ ] process_combo_event() case (if COMBO_ACTION)
- [ ] get_combo_term() case (if not MED)
- [ ] combo_should_trigger() case with layer filter
- [ ] Combo index table updated
- [ ] No timing conflicts with same-key combos
