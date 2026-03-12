---
name: qmk-add-layer
description: Guided walkthrough for adding a new layer to the QMK userspace
disable-model-invocation: true
allowed-tools: Read, Grep, Glob
---

Guide the user through adding a new layer. Read current state first, then walk through every required change.

## Step 1: Gather Requirements

Ask the user:
1. **Layer name** — what to call it (e.g. _MEDIA, _NUMPAD, _ARROWS)
2. **Purpose** — what keys go on it
3. **Which keyboards** — Mitosis, Kyria, Corne, Planck, Zima, or all?
4. **Home row mods?** — Gaming-adjacent layers should NOT have HRM
5. **Switching method** — How to enter/exit:
   - `MO()` (momentary, release to exit)
   - `LT(layer, keycode)` (layer-tap: hold=layer, tap=keycode)
   - `TO()` (permanent switch, need combo or key to exit)
   - `TG()` (toggle on/off)
   - Tri-layer (activated by holding two layers, configured in config.h)
   - Combo (3-finger switch via combos.h)

## Step 2: Determine Layer Index

Read `users/naughtyusername/naughtyusername.h` and find the current last enum entry before `_MOUSE` or at the end.
New layer goes in the enum at the position that makes sense logically. Remember: tri-layer depends on _LOWER=2, _RAISE=3, _ADJUST=5, so don't break those indices.

## Step 3: Walk Through All Required Changes

### 3a. Layer Enum — `users/naughtyusername/naughtyusername.h`

Add to `enum userspace_layers`:
```c
_NEWLAYER,  // Description of purpose
```

Place it logically (gaming layers together, utility layers together).

### 3b. Core 5-Column Macros — `users/naughtyusername/wrappers.h`

Add a comment block following the existing format, then 6 macros:
```c
/* --------------------------------------------------------------------------
 * NEWLAYER LAYER - Description
 * --------------------------------------------------------------------------
 *  _     _     _     _     _          _     _     _     _     _
 *  _     _     _     _     _          _     _     _     _     _
 *  _     _     _     _     _          _     _     _     _     _
 */
#define ___NEWLAYER_L1___ KC_?, KC_?, KC_?, KC_?, KC_?
#define ___NEWLAYER_L2___ KC_?, KC_?, KC_?, KC_?, KC_?
#define ___NEWLAYER_L3___ KC_?, KC_?, KC_?, KC_?, KC_?
#define ___NEWLAYER_R1___ KC_?, KC_?, KC_?, KC_?, KC_?
#define ___NEWLAYER_R2___ KC_?, KC_?, KC_?, KC_?, KC_?
#define ___NEWLAYER_R3___ KC_?, KC_?, KC_?, KC_?, KC_?
```

### 3c. 6-Column Expansion Macros — `users/naughtyusername/wrappers.h`

For Kyria, Corne, and optionally Planck:
```c
#define ___NEWLAYER_L1_6___ OUTER_L, ___NEWLAYER_L1___
#define ___NEWLAYER_L2_6___ OUTER_L, ___NEWLAYER_L2___
#define ___NEWLAYER_L3_6___ OUTER_L, ___NEWLAYER_L3___
#define ___NEWLAYER_R1_6___ ___NEWLAYER_R1___, OUTER_R
#define ___NEWLAYER_R2_6___ ___NEWLAYER_R2___, OUTER_R
#define ___NEWLAYER_R3_6___ ___NEWLAYER_R3___, OUTER_R
```

Check existing layers for outer column patterns (BASE uses ESC/TAB/GUI left, BSPC/QUOT/GUI right).

### 3d. 8-Column Expansion (Kyria only) — `users/naughtyusername/wrappers.h`

Kyria row 3 has 2 extra inner keys:
```c
#define ___NEWLAYER_L3_8___ ___NEWLAYER_L3_6___, _______, _______
#define ___NEWLAYER_R3_8___ _______, _______, ___NEWLAYER_R3_6___
```

### 3e. Thumb Cluster Macros — `users/naughtyusername/wrappers.h`

For each keyboard that needs this layer:

**Mitosis** (4 keys per quadrant, 4 quadrants):
```c
#define ___MITOSIS_THUMB_NEWLAYER_L1___ key, key, key, key
#define ___MITOSIS_THUMB_NEWLAYER_L2___ key, key, key, key
#define ___MITOSIS_THUMB_NEWLAYER_R1___ key, key, key, key
#define ___MITOSIS_THUMB_NEWLAYER_R2___ key, key, key, key
```

**Kyria** (10 keys total, one macro):
```c
#define ___KYRIA_THUMB_NEWLAYER___ \
key, key, key, key, key, \
key, key, key, key, key
```

**Corne** (6 keys total, one macro):
```c
#define ___CORNE_THUMB_NEWLAYER___ key, key, key, key, key, key
```

**Planck** (11 keys + 2u spacebar, one macro):
```c
#define ___PLANCK_BOTTOM_NEWLAYER___ \
    key, key, key, key, key, \
    KC_SPC, \
    key, key, key, key, key
```

**CRITICAL**: If this is a gaming-adjacent layer accessed via `TO()`, use explicit keycodes. Never `_______`.

### 3f. Layer Block in Each keymap.c

Add to the `keymaps[]` array at the correct index position.

**Mitosis** (`keyboards/mitosis/keymaps/naughtyusername/keymap.c`):
```c
[_NEWLAYER] = LAYOUT_mitosis_wrapper(
    ___NEWLAYER_L1___,                      ___NEWLAYER_R1___,
    ___NEWLAYER_L2___,                      ___NEWLAYER_R2___,
    ___NEWLAYER_L3___,                      ___NEWLAYER_R3___,
    ___MITOSIS_THUMB_NEWLAYER_L1___,        ___MITOSIS_THUMB_NEWLAYER_R1___,
    ___MITOSIS_THUMB_NEWLAYER_L2___,        ___MITOSIS_THUMB_NEWLAYER_R2___
),
```

**Kyria** (`keyboards/splitkb/kyria/rev1/keymaps/naughtyusername/keymap.c`):
```c
[_NEWLAYER] = LAYOUT_kyria_wrapper(
    ___NEWLAYER_L1_6___,                    ___NEWLAYER_R1_6___,
    ___NEWLAYER_L2_6___,                    ___NEWLAYER_R2_6___,
    ___NEWLAYER_L3_8___,                    ___NEWLAYER_R3_8___,
                         ___KYRIA_THUMB_NEWLAYER___
),
```

**Corne** (`keyboards/splitkb/halcyon/corne/rev2/keymaps/naughtyusername/keymap.c`):
```c
[_NEWLAYER] = LAYOUT_corne_wrapper(
    ___NEWLAYER_L1_6___,                    ___NEWLAYER_R1_6___,
    ___NEWLAYER_L2_6___,                    ___NEWLAYER_R2_6___,
    ___NEWLAYER_L3_6___,                    ___NEWLAYER_R3_6___,
                         ___CORNE_THUMB_NEWLAYER___
),
```

**Planck** (`keyboards/zsa/planck_ez/glow/keymaps/naughtyusername/keymap.c`):
```c
[_NEWLAYER] = LAYOUT_planck_wrapper(
    ___NEWLAYER_L1_6___,    ___NEWLAYER_R1_6___,
    ___NEWLAYER_L2_6___,    ___NEWLAYER_R2_6___,
    ___NEWLAYER_L3_6___,    ___NEWLAYER_R3_6___,
    ___PLANCK_BOTTOM_NEWLAYER___
),
```

### 3g. Layer Switching

Depending on the chosen method:
- **Combo**: Add to `combos.h` using `/qmk-add-combo`
- **Key binding**: Modify an existing thumb cluster or add `MO(_NEWLAYER)` / `LT(_NEWLAYER, KC_X)` to wrappers
- **Tri-layer**: Configure in `config.h` with `TRI_LAYER_LOWER_LAYER`, `TRI_LAYER_UPPER_LAYER`, `TRI_LAYER_ADJUST_LAYER`

### 3h. Update Existing Combos (if needed)

If existing combos should also be active on this new layer, update their cases in `combo_should_trigger()`.

### 3i. OLED/LED Updates (if applicable)

- **Mitosis**: Add case to `layer_state_set_keymap()` for LED color
- **Kyria**: Add case to `oled_task_user()` for display name
- **Corne Halcyon**: Add case to TFT display layer indicator

## Step 4: Verification Checklist

**users/naughtyusername/naughtyusername.h:**
- [ ] `_NEWLAYER` added to `enum userspace_layers`

**users/naughtyusername/wrappers.h:**
- [ ] 6 core 5-col macros (`___NEWLAYER_[LR][123]___`)
- [ ] 6 expansion macros (`___NEWLAYER_[LR][123]_6___`) — if 6-col boards need it
- [ ] 2 Kyria row 3 macros (`___NEWLAYER_[LR]3_8___`) — if Kyria needs it
- [ ] Mitosis thumb macros (L1/L2/R1/R2, 4 keys each) — if Mitosis needs it
- [ ] Kyria thumb macro (10 keys) — if Kyria needs it
- [ ] Corne thumb macro (6 keys) — if Corne needs it
- [ ] Planck bottom macro (11 keys + 2u space) — if Planck needs it

**Each keymap.c that includes this layer:**
- [ ] Layer block added with correct wrapper macro and index

**users/naughtyusername/combos.h:**
- [ ] Layer switching mechanism added (if combo-based)
- [ ] Existing combo layer filters updated in `combo_should_trigger()` (if needed)

**Board-specific UI:**
- [ ] Mitosis LED color added to `layer_state_set_keymap()`
- [ ] Kyria OLED label added to `oled_task_user()`
