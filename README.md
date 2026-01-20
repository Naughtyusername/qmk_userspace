# QMK Userspace

A unified QMK userspace providing a consistent typing experience across multiple mechanical keyboards with advanced features and ergonomic optimizations.

## Supported Keyboards

- **Mitosis** (46 keys) - 3×5 split ergo with 2×4 thumb clusters
- **Halcyon Corne** (42 keys) - Split with TFT display and Cirque trackpad support  
- **Kyria** (50 keys) - Split with encoders and 5 thumb keys per side
- **Planck EZ Glow** (47 keys) - Ortholinear with RGB matrix
- **Zima** (12 keys) - Macropad with encoder and OLED

## Core Features

- **Home Row Mods (GACS)** - Gui/Alt/Ctrl/Shift on home row with intelligent timing
- **Adaptive Layers** - Consistent layer system across all keyboards:
  - `_BASE` - QWERTY with home row mods
  - `_LOWER` - Numbers and media controls
  - `_RAISE` - Symbols and programming operators
  - `_FUNCTION` - F-keys and navigation
  - `_ADJUST` - System controls (tri-layer activation)
  - `_GAMING` - Gaming layer without home row mods

- **Advanced Typing Features**:
  - Per-key tapping term optimization
  - Flow tap for fast typing prevention
  - Tap dance (Q→Escape, etc.)
  - Key combos (J+K→Escape)
  - Caps Word with 3s timeout

- **Programming Keycodes** - Custom shortcuts for common operators:
  - `:=` assignment operator
  - `->` arrow operator  
  - `::` scope resolution
  - `..=` range operator
  - `~/` home directory

## Architecture

The userspace uses QMK's modular architecture to share code across keyboards:

- `naughtyusername.h/c` - Core shared functionality
- `keyrecords.c` - Tap dances, combos, and key overrides
- `wrappers.h` - Layout macros for different form factors
- Layout wrapper system adapts 3×5 alpha grid to each keyboard's thumb cluster

## Build Targets

```bash
# Main userspace builds
qmk compile -kb mitosis -km naughtyusername
qmk compile -kb splitkb/kyria/rev1 -km naughtyusername
qmk compile -kb zsa/planck_ez/glow -km naughtyusername
qmk compile -kb splitkb/zima -km naughty_zima
TODO: add in the system we use for the halcyon board and explain how to build it with 2 different modules for anyone in the future who could use that advice

# Halcyon module builds (with display/encoder/trackpad options)
qmk compile -kb splitkb/halcyon/kyria/rev4 -km vial_hlc
qmk compile -kb splitkb/halcyon/corne/rev2 -km vial_hlc
# ... and other Halcyon variants
```

## Key Technical Highlights

- Weak function pattern for keymap-specific overrides
- Tri-layer support for advanced layer combinations
- Chordal hold configuration for improved home row mod behavior
- Comprehensive documentation and code organization
- Support for both 5-column and 6-column layouts

## License

GPL-2.0-or-later - See QMK project license for details.
