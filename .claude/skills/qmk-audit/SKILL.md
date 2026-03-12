---
name: qmk-audit
description: Pre-build validation for QMK userspace — checks conventions, macro consistency, combo completeness, and structural errors before compiling
disable-model-invocation: true
allowed-tools: Grep, Read, Glob
---

Audit the QMK userspace for convention violations and structural errors. Run ALL checks below and report findings.

## Check 1: Gaming Thumb Clusters — No `_______`

Gaming layers use `TO()` (permanent switch), so `_______` falls through to BASE where home row mods add latency. Every gaming/roguelike thumb must be explicit.

Search `users/naughtyusername/wrappers.h` for these macros and flag any `_______`:
- `___MITOSIS_THUMB_GAMING_*___`, `___MITOSIS_THUMB_GAMING2_*___`, `___MITOSIS_THUMB_ROGUELIKE_*___`
- `___KYRIA_THUMB_GAMING___`, `___KYRIA_THUMB_ROGUELIKE___`
- `___CORNE_THUMB_GAMING___`, `___CORNE_THUMB_ROGUELIKE___`
- `___PLANCK_BOTTOM_GAMING___`, `___PLANCK_BOTTOM_ROGUELIKE___`

Severity: **ERROR** — causes input latency on gaming layers.

## Check 2: Combo Enum Completeness

Every entry in `enum combo_names` (in `combos.h`) must have ALL of:
1. A `const uint16_t PROGMEM` key array
2. An entry in `key_combos[]` (either `COMBO()` or `COMBO_ACTION()`)
3. A case in `combo_should_trigger()` (or fall through to default `return true`)

Read `combos.h` and verify:
- Count enum entries (excluding COMBO_LENGTH)
- Count key_combos[] entries
- Count combo_should_trigger() cases
- Flag any mismatches

Also check: every `COMBO_ACTION()` entry has a corresponding case in `process_combo_event()`.

Severity: **ERROR** — missing entries cause array bounds issues or combos firing on wrong layers.

## Check 3: Combo Timing Consistency

Combos sharing the same physical keys MUST have the same timeout in `get_combo_term()`.

Known shared-key pairs (same PROGMEM array, layer-filtered):
- `combo_qw` → COMBO_QW_OSM_LGUI + COMBO_QW_NEQL
- `combo_er` → COMBO_ER_OSM_LCTL + COMBO_ER_REPEAT
- `combo_ui` → COMBO_UI_OSM_RCTL + COMBO_UI_EQUAL
- `combo_io` → COMBO_IO_OSM_RALT + COMBO_IO_COLON
- `combo_yui` → COMBO_YUI_TO_GAMING2 + COMBO_YUI_TO_GAMING
- `combo_hjk` → COMBO_HJK_TO_ROGUELIKE + COMBO_HJK_TO_GAMING
- `combo_nm_comm` → COMBO_NM_COMM_TO_VIM + COMBO_NM_COMM_TO_BASE

Verify each pair has identical timing in `get_combo_term()`. Different timing on same keys causes one combo's early resolution to swallow keypresses.

Severity: **ERROR** — causes missed keystrokes.

## Check 4: Layer Completeness Across Keymaps

Read each `keymap.c` and verify it has layer entries for all expected layers.

Expected layers per keyboard:
- **Mitosis**: All 11 (_BASE through _MOUSE)
- **Kyria**: 9 layers (no _GAMING2, no _MOUSE — intentionally omitted, verify comment exists)
- **Corne Halcyon**: All 11 (has _MOUSE for cirque trackpad)
- **Planck**: Check which layers are present
- **Zima**: Macropad, different layer set

For keyboards with all 11 layers, verify the `[_LAYER]` indices in `keymaps[]` are correct and no layer is accidentally omitted.

Severity: **ERROR** — missing layer means TO() switches to an empty layer.

## Check 5: Undefined Macro References

In each `keymap.c`, find all `___*___` wrapper macro references.
Verify each one has a corresponding `#define` in `wrappers.h`.

Common patterns to check:
- Mitosis uses 5-col macros + `___MITOSIS_THUMB_*___`
- Kyria uses 6-col macros + 8-col row3 macros + `___KYRIA_THUMB_*___`
- Corne uses 6-col macros + `___CORNE_THUMB_*___`
- Planck uses 5-col macros + `___PLANCK_BOTTOM_*___`

Severity: **ERROR** — undefined macro = preprocessor error.

## Check 6: Custom Keycode Handling

Every entry in `enum userspace_keycodes` (in `naughtyusername.h`) that isn't `NEW_SAFE_RANGE` should have a case in `process_record_user()` (in `naughtyusername.c`).

Flag any keycodes defined but not handled.

Severity: **WARN** — dead keycode, wastes firmware space.

## Check 7: rules.mk Feature Flag Consistency

Check that features enabled in `users/naughtyusername/rules.mk` have corresponding code:
- `COMBO_ENABLE = yes` → `combos.h` exists and is included via `keyrecords.c`
- `LEADER_ENABLE = yes` → leader sequences defined somewhere
- `KEY_OVERRIDE_ENABLE = yes` → key_overrides[] array exists
- `CAPS_WORD_ENABLE = yes` → caps_word config in config.h

Also check that `keyrecords.c` is `#include`d (not compiled) in each `keymap.c` — QMK introspection requires it.

Severity: **WARN** — orphaned feature flags bloat firmware.

## Check 8: HRM Key Array Correctness

In `combos.h`, PROGMEM key arrays for home-row positions MUST use `HM_*` macros (not bare `KC_*`).
Because `COMBO_ONLY_FROM_LAYER 0` checks against BASE layer keycodes, and BASE uses home row mods.

Check all arrays that reference A, S, D, F, J, K, L, SCLN positions:
- `combo_hj` should use `KC_H, HM_J` (H has no HRM, J does)
- `combo_as` should use `HM_A, HM_S`
- etc.

Severity: **ERROR** — wrong keycode means combo never fires.

## Report Format

```
=== QMK Userspace Audit ===

[PASS/FAIL] Check 1: Gaming Thumb Clusters
  (details if FAIL)

[PASS/FAIL] Check 2: Combo Enum Completeness
  (details if FAIL)

... etc ...

Summary: X/8 checks passed, Y errors, Z warnings
Files verified: naughtyusername.h, wrappers.h, combos.h, naughtyusername.c, config.h, rules.mk, all keymap.c files
```
