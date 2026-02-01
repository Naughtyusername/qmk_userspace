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

This userspace provides a unified experience across various keyboards. To compile your firmware, use the `qmk compile` command with the appropriate keyboard and keymap.

### Main Userspace Builds

These commands compile the firmware for each supported keyboard using the `naughtyusername` keymap, inheriting all shared features from `users/naughtyusername/`.

```bash
qmk compile -kb mitosis -km naughtyusername
qmk compile -kb splitkb/kyria/rev1 -km naughtyusername
qmk compile -kb zsa/planck_ez/glow -km naughtyusername
qmk compile -kb splitkb/zima -km naughty_zima # Note: Zima uses 'naughty_zima' keymap due to its standalone nature
```

### Halcyon Module Builds

Halcyon keyboards (e.g., Corne, Kyria, Elora, Lily58) can leverage modular components like TFT displays, encoders, and Cirque trackpads. These modules are enabled by passing environment variables during the `qmk compile` command. The base Halcyon keymap will automatically include the necessary userspace modules.

To enable specific modules, use the `-e` flag with `HLC_TFT_DISPLAY=yes`, `HLC_ENCODER=yes`, or `HLC_CIRQUE_TRACKPAD=yes`. You can combine multiple modules.

For example, to build for a Halcyon Corne with a TFT display and a Cirque trackpad:

```bash
qmk compile -kb splitkb/halcyon/corne/rev2 -km naughtyusername -e HLC_TFT_DISPLAY=yes -e HLC_CIRQUE_TRACKPAD=yes
```

To build for a Halcyon Kyria with only an encoder:

```bash
qmk compile -kb splitkb/halcyon/kyria/rev4 -km naughtyusername -e HLC_ENCODER=yes
```

Make sure to specify the correct keyboard (`-kb`) for your Halcyon variant.


## Key Technical Highlights

- Weak function pattern for keymap-specific overrides
- Tri-layer support for advanced layer combinations
- Chordal hold configuration for improved home row mod behavior
- Comprehensive documentation and code organization
- Support for both 5-column and 6-column layouts

## License

GPL-2.0-or-later - See QMK project license for details.
