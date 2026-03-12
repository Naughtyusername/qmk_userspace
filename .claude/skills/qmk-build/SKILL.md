---
name: qmk-build
description: Build QMK firmware for a specific keyboard or all keyboards
disable-model-invocation: true
argument-hint: mitosis|kyria|corne|planck|zima|all
allowed-tools: Bash(qmk *), Bash(cd *), Bash(ls *), Bash(bash *), Read
---

Build QMK firmware. Parse `$ARGUMENTS` to determine target:

- `/qmk-build mitosis` — build Mitosis
- `/qmk-build kyria` — build Kyria rev1
- `/qmk-build corne` — build Halcyon Corne (both halves)
- `/qmk-build planck` — build Planck EZ Glow
- `/qmk-build zima` — build Zima macropad
- `/qmk-build all` — build everything

If `$ARGUMENTS` is empty or unrecognized, ask the user which keyboard to build.

## Build Commands

All commands run from the qmk_userspace directory: `$HOME/dev/firmware/qmk_userspace`

**Standard keyboards:**
```bash
qmk compile -kb mitosis -km naughtyusername
qmk compile -kb splitkb/kyria/rev1 -km naughtyusername
qmk compile -kb zsa/planck_ez/glow -km naughtyusername
qmk compile -kb splitkb/zima -km naughty_zima
```

**Halcyon Corne (split, needs separate builds per half):**
```bash
# Left half: TFT Display
qmk compile -kb splitkb/halcyon/corne/rev2 -km naughtyusername -e HLC_TFT_DISPLAY=1 -e TARGET=corne_left_display

# Right half: Cirque Trackpad
qmk compile -kb splitkb/halcyon/corne/rev2 -km naughtyusername -e HLC_CIRQUE_TRACKPAD=1 -e TARGET=corne_right_trackpad
```

**All keyboards** — run `bash build_userspace.sh` from the qmk_userspace directory, or build each one sequentially. Continue building remaining keyboards even if one fails.

## After Build

On success:
1. List firmware files (`.hex`, `.bin`, `.uf2`) with sizes
2. Report which output format each keyboard produces

On failure:
1. Show the full compiler error output
2. **Trace macro errors**: If the error references an expanded macro line, read `users/naughtyusername/wrappers.h` and identify which `#define` wrapper contains the problematic binding
3. Suggest a fix based on common patterns:
   - "implicit declaration" → missing `#include` or undefined custom keycode
   - "too many/few initializers" → wrong key count in a wrapper macro, count keys in the define
   - "use of undeclared identifier" → typo in macro name, search wrappers.h
   - "expected expression" → syntax error in macro (missing comma between keys)
   - Halcyon errors → check if the correct `-e` flags are passed

## Keyboard → Keymap Name Mapping

| Shorthand | -kb flag | -km flag |
|-----------|----------|----------|
| mitosis | `mitosis` | `naughtyusername` |
| kyria | `splitkb/kyria/rev1` | `naughtyusername` |
| corne | `splitkb/halcyon/corne/rev2` | `naughtyusername` |
| planck | `zsa/planck_ez/glow` | `naughtyusername` |
| zima | `splitkb/zima` | `naughty_zima` |
