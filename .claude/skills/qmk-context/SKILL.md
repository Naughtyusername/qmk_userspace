---
name: qmk-context
description: Load QMK userspace context when discussion involves layers, keymaps, combos, wrappers, thumb clusters, behaviors, home row mods, tap dance, key overrides, or build configuration in this qmk_userspace repository
disable-model-invocation: false
allowed-tools: Read, Glob, Grep
---

Load the QMK userspace context by reading key structural files. Internalize this reference — do NOT dump it to the user unless they ask.

**Step 1: Read core files for current state**
- `users/naughtyusername/naughtyusername.h` — layer enum, custom keycodes, HRM defines, layer-tap shortcuts
- `users/naughtyusername/wrappers.h` — all layout macros, thumb clusters, 6-col/8-col expansions
- `users/naughtyusername/combos.h` — combo enum, key arrays, key_combos[], callbacks, layer filtering
- `users/naughtyusername/config.h` — timing config, feature flags
- `users/naughtyusername/rules.mk` — build flags, source files

**Step 2: Internalize this reference**

Layer enum (defined in naughtyusername.h via `enum userspace_layers`):
| Index | Name        | Purpose                                    |
|-------|-------------|--------------------------------------------|
| 0     | _BASE       | QWERTY + GACS home row mods (daily driver) |
| 1     | _VIM        | Plain QWERTY, no HRM (vimium, editors)     |
| 2     | _LOWER      | Left=symbols (mirror RAISE), Right=numpad  |
| 3     | _RAISE      | Symbols (Odin/programming focused)         |
| 4     | _FUNCTION   | F-keys + navigation                        |
| 5     | _ADJUST     | Tri-layer (LOWER+RAISE), media, system     |
| 6     | _GAMING     | Plain QWERTY, no HRM, explicit thumbs      |
| 7     | _GAMING2    | F-keys/numbers overlay for gaming          |
| 8     | _ROGUELIKE  | Gaming + numpad right hand                 |
| 9     | _SYS        | Bootloader, EE_CLR, NKRO toggle            |
| 10    | _MOUSE      | Mouse keys for Halcyon Corne trackpad      |

Note: QMK uses `_FUNCTION` (not `_FUNC` like ZMK). Layer names have underscore prefix.

Macro naming convention (same as ZMK, Drashna-style):
- 5-col core: `___LAYER_ROW_SIDE___` (e.g. `___BASE_L1___`, `___GAMING_R3___`)
- 6-col expansion: `___LAYER_ROW_SIDE_6___` (adds outer column)
- 8-col expansion: `___LAYER_ROW_SIDE_8___` (Kyria row 3 — adds 2 inner keys)

Keyboards and their wrapper macros:
| Keyboard | Keys | Wrapper | Rows | Thumbs | Notes |
|----------|------|---------|------|--------|-------|
| Mitosis | 46 | `LAYOUT_mitosis_wrapper` | 5-col | `___MITOSIS_THUMB_*_L1/L2/R1/R2___` (4 per quadrant) | Wireless, RGB LED |
| Kyria rev1 | 50 | `LAYOUT_kyria_wrapper` | 6-col + 8-col row3 | `___KYRIA_THUMB_*___` (10 total) | Encoders, OLED |
| Corne Halcyon | 42 | `LAYOUT_corne_wrapper` | 6-col | `___CORNE_THUMB_*___` (6 total) | TFT display (L), Cirque trackpad (R) |
| Planck EZ | 47 | `LAYOUT_planck_wrapper` | 5-col + outer | `___PLANCK_BOTTOM_*___` (11 total, 2u space) | Ortholinear |
| Zima | 12 | `LAYOUT_zima_wrapper` | macropad | N/A | Encoder macropad |

Custom keycodes (from `enum userspace_keycodes`):
- `KC_ASSIGN` (:=), `KC_ARROP` (->), `KC_DCLN` (::), `KC_RANGE` (..=), `KC_HMDR` (~/)
- `KC_LTEQ` (<=), `KC_GTEQ` (>=), `KC_EQEQ` (==)
- `NUMWORD` — combo-activated auto-layer for numbers

Home row mods (GACS order):
- Left: `HM_A` (LGUI), `HM_S` (LALT), `HM_D` (LCTL), `HM_F` (LSFT)
- Right: `HM_J` (RSFT), `HM_K` (RCTL), `HM_L` (RALT), `HM_SCLN` (RGUI)

Layer-tap shortcuts: `SP_RAI`, `SP_LOW`, `ENT_LOW`, `FUN_TAB`, `LOW`, `RAI`, `FUN`, `ADJ`, `GAM`

QMK combo system (differs from ZMK):
- `COMBO_ONLY_FROM_LAYER 0` — combos match by BASE layer keycodes, not positions
- No per-board position overrides needed (unlike ZMK's per-keymap key-positions)
- Adding a combo requires: enum entry, PROGMEM key array, key_combos[] entry, optionally process_combo_event() case, get_combo_term() case, combo_should_trigger() case
- Same keys can have different combos on different layers via `combo_should_trigger()`
- Timeout tiers: COMBO_FAST=18ms, COMBO_MED=30ms, COMBO_SLOW=50ms

Gaming layer rules (same as ZMK):
- Gaming combos use `TO()` (permanent switch)
- `_______` in a TO()-switched layer falls through to BASE (HRM latency)
- **Every gaming/roguelike thumb must use explicit keycodes, never `_______`**

Build system:
- `qmk compile -kb <keyboard> -km <keymap>` for individual builds
- `build_userspace.sh` builds all keyboards including Halcyon split halves
- Halcyon Corne needs separate builds per half: `-e HLC_TFT_DISPLAY=1` (left), `-e HLC_CIRQUE_TRACKPAD=1` (right)
- `qmk.json` defines build targets for `qmk userspace-compile`

File responsibilities:
| File | Owns |
|------|------|
| `users/naughtyusername/naughtyusername.h` | Layer enum, custom keycodes, HRM defines, includes wrappers.h |
| `users/naughtyusername/naughtyusername.c` | process_record_user, tapping term per key, chordal hold, flow tap |
| `users/naughtyusername/wrappers.h` | All layout macros, thumb clusters, 6/8-col expansions |
| `users/naughtyusername/combos.h` | Combo enum, key arrays, key_combos[], all combo callbacks |
| `users/naughtyusername/keyrecords.c` | Included in each keymap.c for QMK introspection |
| `users/naughtyusername/config.h` | Timing (tapping term, combo term, flow tap), feature config |
| `users/naughtyusername/rules.mk` | Feature flags, source files, Halcyon module integration |
| `users/naughtyusername/numword.h/.c` | Num Word auto-layer implementation |
| `keyboards/*/keymaps/*/keymap.c` | Board-specific layer assembly using wrapper macros |
| `build_userspace.sh` | Build all keyboards script |
| `qmk.json` | Build targets for `qmk userspace-compile` |

**Step 3: Read files relevant to the user's specific question before responding.**
