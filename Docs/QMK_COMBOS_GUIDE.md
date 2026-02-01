# QMK Combos: A Deep Dive + Modular Implementation

## Table of Contents
1. [What Are Combos?](#what-are-combos)
2. [How Combos Work Internally](#how-combos-work-internally)
3. [Basic Combo Anatomy](#basic-combo-anatomy)
4. [Combo Types](#combo-types)
5. [Configuration Options](#configuration-options)
6. [Per-Combo Customization](#per-combo-customization)
7. [Combos with Home Row Mods](#combos-with-home-row-mods)
8. [Layer-Independent Combos](#layer-independent-combos)
9. [Modular Implementation for Your System](#modular-implementation-for-your-system)

---

## What Are Combos?

Combos are a **chording** feature - press multiple keys simultaneously (within a time window) 
to produce a different output. Think of it like a piano chord: press A+B together → get Escape.

**Common use cases:**
- `J + K` → Escape (Vim-style)
- `D + F` → Tab
- `S + D + F` → Caps Word
- `E + R` → Backspace (easier reach)
- Horizontal combos for brackets: `U + I` → `(`, `I + O` → `)`

---

## How Combos Work Internally

### The Timeline

```
Time ──────────────────────────────────────────────────────────>

     Key A pressed     Key B pressed      COMBO_TERM expires
          │                 │                    │
          ▼                 ▼                    ▼
     ┌─────────────────────────────────────────────┐
     │            COMBO_TERM (default 50ms)        │
     └─────────────────────────────────────────────┘
          │                 │                    │
          │    Both keys    │     Combo fires!   │
          │    buffered     │                    │
```

1. **First key press**: Timer starts, key is buffered (not sent yet)
2. **Subsequent key presses**: If within `COMBO_TERM`, keys are buffered
3. **Combo detected**: If buffered keys match a combo definition, fire the combo
4. **Timeout**: If `COMBO_TERM` expires without a match, buffered keys are sent normally

### Key Insight: Combos Buffer Keys

This is crucial to understand: **combos intercept and buffer keypresses**. If you press `J`, 
QMK doesn't immediately send `J` - it waits to see if you're starting a combo. This can 
cause a slight delay in typing, which is why tuning `COMBO_TERM` matters.

---

## Basic Combo Anatomy

### Minimum Required

```c
// 1. Enable in rules.mk
COMBO_ENABLE = yes

// 2. In your keymap.c (or keyrecords.c in your case)

// Define the key sequence (MUST be in PROGMEM, MUST end with COMBO_END)
const uint16_t PROGMEM jk_combo[] = {KC_J, KC_K, COMBO_END};

// Define the combo array (QMK finds this by name - MUST be called key_combos)
combo_t key_combos[] = {
    COMBO(jk_combo, KC_ESC),  // J+K = Escape
};
```

### The Three Pieces

1. **Key Array**: `const uint16_t PROGMEM xxx_combo[] = {..., COMBO_END};`
   - Must be in `PROGMEM` (stored in flash, not RAM)
   - Must end with `COMBO_END` sentinel
   - Order doesn't matter by default (but can be enforced)

2. **Combo Definition**: Uses one of the combo macros
   - `COMBO(keys, keycode)` - simple keycode output
   - `COMBO_ACTION(keys)` - custom action via `process_combo_event`

3. **The `key_combos` Array**: QMK looks for this specific name
   - Can optionally use an enum for indexing

---

## Combo Types

### Type 1: Simple Keycode

```c
const uint16_t PROGMEM jk_combo[] = {KC_J, KC_K, COMBO_END};
combo_t key_combos[] = {
    COMBO(jk_combo, KC_ESC),           // Basic keycode
    COMBO(df_combo, KC_TAB),           // Another basic
    COMBO(ui_combo, KC_LPRN),          // Symbols work
    COMBO(io_combo, LCTL(KC_Z)),       // Modifiers work too!
    COMBO(qw_combo, LSFT(KC_1)),       // Shift+1 = !
};
```

### Type 2: Layer Keys

```c
const uint16_t PROGMEM nav_combo[] = {KC_S, KC_D, KC_F, COMBO_END};
combo_t key_combos[] = {
    COMBO(nav_combo, MO(_NAV)),        // Momentary layer
    COMBO(tog_combo, TG(_GAMING)),     // Toggle layer
    COMBO(osl_combo, OSL(_SYMBOLS)),   // One-shot layer
};
```

### Type 3: Custom Actions (COMBO_ACTION)

For anything more complex than a keycode:

```c
enum combo_events {
    COMBO_EMAIL,
    COMBO_ARROW,
    COMBO_CLEAR_LINE,
};

const uint16_t PROGMEM email_combo[] = {KC_E, KC_M, COMBO_END};
const uint16_t PROGMEM arrow_combo[] = {KC_MINS, KC_DOT, COMBO_END};  // -> arrow

combo_t key_combos[] = {
    [COMBO_EMAIL] = COMBO_ACTION(email_combo),
    [COMBO_ARROW] = COMBO_ACTION(arrow_combo),
};

void process_combo_event(uint16_t combo_index, bool pressed) {
    switch(combo_index) {
        case COMBO_EMAIL:
            if (pressed) {
                SEND_STRING("your.email@example.com");
            }
            break;
        case COMBO_ARROW:
            if (pressed) {
                SEND_STRING("->");  // Great for Odin!
            }
            break;
    }
}
```

### Type 4: String Macros (via gboards helper)

If you use the gboards `combos.def` system:

```c
// In combos.def
SUBS(ARROW_COMBO, "->", KC_MINS, KC_DOT)
SUBS(FAT_ARROW,   "=>", KC_EQL, KC_DOT)
SUBS(SCOPE,       "::", KC_SCLN, KC_SCLN)  // For Odin!
```

---

## Configuration Options

### In config.h

```c
/* ========== TIMING ========== */
#define COMBO_TERM 50              // Default. Time window to complete combo (ms)
#define COMBO_HOLD_TERM 150        // For COMBO_MUST_HOLD_MODS (default: TAPPING_TERM)

/* ========== BEHAVIOR FLAGS ========== */
#define COMBO_MUST_HOLD_MODS       // Modifier combos must be held, not tapped
#define COMBO_MUST_PRESS_IN_ORDER  // Keys must be pressed in definition order
#define COMBO_STRICT_TIMER         // Timer only starts on first key (stricter)
#define COMBO_NO_TIMER             // No timer - fires on first key release

/* ========== BUFFER SIZES ========== */
#define COMBO_KEY_BUFFER_LENGTH 8  // Max keys buffered (default: 8)
#define COMBO_BUFFER_LENGTH 4      // Max simultaneous combos (default: 4)

/* ========== COMBO SIZE ========== */
// Choose ONE based on your longest combo:
#define EXTRA_SHORT_COMBOS         // Max 6 keys per combo (saves memory)
// (default)                       // Max 8 keys per combo
#define EXTRA_LONG_COMBOS          // Max 16 keys per combo
#define EXTRA_EXTRA_LONG_COMBOS    // Max 32 keys per combo

/* ========== LAYER HANDLING ========== */
#define COMBO_ONLY_FROM_LAYER 0    // Always check combos from layer 0
```

### In rules.mk

```makefile
COMBO_ENABLE = yes

# Optional: For the gboards combos.def system
VPATH += keyboards/gboards
```

---

## Per-Combo Customization

These give you fine-grained control over individual combos.

### Per-Combo Timing

```c
// In config.h
#define COMBO_TERM_PER_COMBO

// In keymap/keyrecords
uint16_t get_combo_term(uint16_t combo_index, combo_t *combo) {
    switch (combo_index) {
        case COMBO_JK_ESC:
            return 30;   // Fast typists need shorter term for common combos
        case COMBO_SDF_CAPSWORD:
            return 80;   // 3-key combos need more time
        default:
            return COMBO_TERM;
    }
}
```

### Per-Combo Must-Hold (Great for Layer Combos)

```c
// In config.h
#define COMBO_MUST_HOLD_PER_COMBO

// In keymap/keyrecords
bool get_combo_must_hold(uint16_t combo_index, combo_t *combo) {
    // All layer combos and modifier combos should require holding
    if (KEYCODE_IS_MOD(combo->keycode) ||
        (combo->keycode >= QK_MOMENTARY && combo->keycode <= QK_MOMENTARY_MAX)) {
        return true;
    }
    return false;
}
```

### Per-Combo Must-Tap (Critical for Home Row Mods!)

```c
// In config.h
#define COMBO_MUST_TAP_PER_COMBO

// In keymap/keyrecords
bool get_combo_must_tap(uint16_t combo_index, combo_t *combo) {
    // Make ALL combos tap-only if they contain mod-taps or layer-taps
    uint16_t key;
    uint8_t idx = 0;
    while ((key = pgm_read_word(&combo->keys[idx])) != COMBO_END) {
        switch (key) {
            case QK_MOD_TAP...QK_MOD_TAP_MAX:
            case QK_LAYER_TAP...QK_LAYER_TAP_MAX:
            case QK_MOMENTARY...QK_MOMENTARY_MAX:
                return true;
        }
        idx += 1;
    }
    return false;
}
```

### Conditional Combo Activation

```c
// In config.h
#define COMBO_SHOULD_TRIGGER

// In keymap/keyrecords
bool combo_should_trigger(uint16_t combo_index, combo_t *combo, 
                          uint16_t keycode, keyrecord_t *record) {
    // Disable all combos on gaming layer
    if (layer_state_is(_GAMING)) {
        return false;
    }
    
    // Disable specific combos on certain layers
    switch (combo_index) {
        case COMBO_JK_ESC:
            // Don't fire JK escape in Vim layer (Vim uses j/k for navigation)
            if (layer_state_is(_VIM)) {
                return false;
            }
    }
    
    return true;
}
```

---

## Combos with Home Row Mods

This is the tricky part. You have home row mods like `HM_F` which is `LSFT_T(KC_F)`.

### The Problem

If you define a combo as:
```c
const uint16_t PROGMEM df_combo[] = {KC_D, KC_F, COMBO_END};  // WRONG!
```

This won't work because your keymap has `HM_D` and `HM_F`, not `KC_D` and `KC_F`.

### The Solution

**Option A: Use the actual keycodes from your keymap**

```c
// Use your home row mod keycodes
const uint16_t PROGMEM df_combo[] = {HM_D, HM_F, COMBO_END};  // Correct!
```

**Option B: Use COMBO_ONLY_FROM_LAYER (Recommended)**

```c
// In config.h
#define COMBO_ONLY_FROM_LAYER _BASE  // or just 0
```

This tells QMK: "When checking combos, always look at layer 0's keycodes."
Now you can define combos with the base layer keycodes:

```c
const uint16_t PROGMEM df_combo[] = {HM_D, HM_F, COMBO_END};
```

And it will work even when you're on other layers (as long as D and F are 
in the same physical positions).

**Option C: Use COMBO_MUST_TAP_PER_COMBO**

This is crucial if you want combos to play nice with home row mods:

```c
#define COMBO_MUST_TAP_PER_COMBO

bool get_combo_must_tap(uint16_t combo_index, combo_t *combo) {
    return true;  // All combos are tap-only
}
```

With tap-only combos:
- **Tap** the combo keys → combo fires
- **Hold** the combo keys → no combo, keys act individually (mods activate)

This prevents the annoying situation where you're trying to hold Shift+D 
but accidentally fire the D+F combo.

---

## Layer-Independent Combos

Since you have multiple base layers (BASE, GAMING, VIM, etc.), you want combos 
to work based on physical position, not the keycode on that layer.

### Method 1: COMBO_ONLY_FROM_LAYER

```c
// In config.h
#define COMBO_ONLY_FROM_LAYER 0  // Always check from layer 0 (_BASE)
```

Now all your combos are defined using _BASE layer keycodes, and they work 
regardless of which layer is active.

### Method 2: Reference Layer Per Layer

```c
// In config.h - enable the feature
// Then define the mapping

uint8_t combo_ref_from_layer(uint8_t layer) {
    switch (layer) {
        case _GAMING:  return _BASE;     // Gaming uses BASE combo positions
        case _VIM:     return _BASE;     // VIM uses BASE combo positions  
        case _RAISE:   return _RAISE;    // RAISE has its own combos
        default:       return _BASE;     // Everything else uses BASE
    }
}
```

---

## Modular Implementation for Your System

Now let's make this work with your wrapper system. The goal is:
1. Define combos once in userspace
2. Include in each keymap via `#include "keyrecords.c"`
3. Use your existing keycodes (HM_A, HM_S, etc.)

### File Structure

```
users/naughtyusername/
├── naughtyusername.h      # Your main header
├── naughtyusername.c      # Your main source  
├── wrappers.h             # Layer definitions (existing)
├── keyrecords.c           # Combos, tap dance, key overrides (existing)
└── combos.h               # NEW: Combo definitions
```

### Step 1: Create combos.h

```c
/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * combos.h - Shared combo definitions
 *
 * These combos use the keycodes from your base layer (with home row mods).
 * They're position-based thanks to COMBO_ONLY_FROM_LAYER.
 */

#pragma once

#include "naughtyusername.h"

/* ==========================================================================
 * COMBO CONFIGURATION
 * ==========================================================================
 * Put these in your config.h or here with include guards
 */

// Timing
#ifndef COMBO_TERM
#    define COMBO_TERM 40  // 40ms window - adjust to taste
#endif

// Make combos position-based (check from BASE layer)
#ifndef COMBO_ONLY_FROM_LAYER
#    define COMBO_ONLY_FROM_LAYER _BASE
#endif

// Per-combo customization flags
#define COMBO_MUST_TAP_PER_COMBO      // Important for home row mods
#define COMBO_SHOULD_TRIGGER          // For layer-based disabling

/* ==========================================================================
 * COMBO INDEX ENUM
 * ==========================================================================
 * Naming convention: COMBO_<keys>_<result>
 * Using an enum lets us reference combos by name in callbacks
 */
enum combo_names {
    // Escape combos
    COMBO_JK_ESC,           // J + K = Escape (Vim-style)
    
    // Navigation/editing
    COMBO_DF_TAB,           // D + F = Tab
    COMBO_SD_BSPC,          // S + D = Backspace
    COMBO_KL_ENT,           // K + L = Enter
    
    // Brackets (horizontal combos on top row)
    COMBO_ER_LPRN,          // E + R = (
    COMBO_UI_RPRN,          // U + I = )
    COMBO_WE_LBRC,          // W + E = [
    COMBO_IO_RBRC,          // I + O = ]
    COMBO_QW_LCBR,          // Q + W = {
    COMBO_OP_RCBR,          // O + P = }
    
    // Programming operators (for Odin!)
    COMBO_MCOMM_ARROW,      // , + . = ->
    COMBO_DOTSLSH_DARROW,   // . + / = =>  (or :: for Odin)
    COMBO_COMMM_DCLN,       // M + , = ::  (scope resolution)
    
    // Caps Word (3-key combo)
    COMBO_SDF_CAPSWORD,     // S + D + F = Caps Word
    
    // Layer access
    COMBO_ZX_SYS,           // Z + X = SYS layer (momentary)
    COMBO_SLASHDOT_MOUSE,   // / + . = MOUSE layer
    
    // Utility
    COMBO_CV_PASTE,         // C + V = Ctrl+V (paste)
    COMBO_XC_CUT,           // X + C = Ctrl+X (cut)
    COMBO_ZA_UNDO,          // Z + A = Ctrl+Z (undo)
    
    COMBO_LENGTH            // MUST be last - used for array size
};

/* ==========================================================================
 * COMBO KEY ARRAYS
 * ==========================================================================
 * These define which keys trigger each combo.
 * 
 * IMPORTANT: Use your actual keycodes (HM_D, HM_F, etc.) not KC_D, KC_F
 * The COMBO_ONLY_FROM_LAYER setting handles the layer checking.
 *
 * All arrays MUST:
 *   - Be const uint16_t PROGMEM
 *   - End with COMBO_END
 */

// Escape
const uint16_t PROGMEM combo_jk_esc[] = {HM_J, HM_K, COMBO_END};

// Navigation/editing  
const uint16_t PROGMEM combo_df_tab[] = {HM_D, HM_F, COMBO_END};
const uint16_t PROGMEM combo_sd_bspc[] = {HM_S, HM_D, COMBO_END};
const uint16_t PROGMEM combo_kl_ent[] = {HM_K, HM_L, COMBO_END};

// Brackets (using regular keycodes for top row - no HRM there)
const uint16_t PROGMEM combo_er_lprn[] = {KC_E, KC_R, COMBO_END};
const uint16_t PROGMEM combo_ui_rprn[] = {KC_U, KC_I, COMBO_END};
const uint16_t PROGMEM combo_we_lbrc[] = {KC_W, KC_E, COMBO_END};
const uint16_t PROGMEM combo_io_rbrc[] = {KC_I, KC_O, COMBO_END};
const uint16_t PROGMEM combo_qw_lcbr[] = {KC_Q, KC_W, COMBO_END};
const uint16_t PROGMEM combo_op_rcbr[] = {KC_O, KC_P, COMBO_END};

// Programming operators
const uint16_t PROGMEM combo_mcomm_arrow[] = {KC_M, KC_COMM, COMBO_END};
const uint16_t PROGMEM combo_dotslsh_darrow[] = {KC_DOT, KC_SLSH, COMBO_END};
const uint16_t PROGMEM combo_commm_dcln[] = {KC_COMM, KC_M, COMBO_END};

// Caps Word (3 keys)
const uint16_t PROGMEM combo_sdf_capsword[] = {HM_S, HM_D, HM_F, COMBO_END};

// Layer access
const uint16_t PROGMEM combo_zx_sys[] = {KC_Z, KC_X, COMBO_END};
const uint16_t PROGMEM combo_slashdot_mouse[] = {KC_SLSH, KC_DOT, COMBO_END};

// Utility
const uint16_t PROGMEM combo_cv_paste[] = {KC_C, KC_V, COMBO_END};
const uint16_t PROGMEM combo_xc_cut[] = {KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM combo_za_undo[] = {KC_Z, HM_A, COMBO_END};

/* ==========================================================================
 * COMBO DEFINITIONS ARRAY
 * ==========================================================================
 * This MUST be named `key_combos` - QMK looks for this specific name.
 * The [COMBO_NAME] = syntax ties each combo to its enum index.
 */
combo_t key_combos[COMBO_LENGTH] = {
    // Escape
    [COMBO_JK_ESC]         = COMBO(combo_jk_esc, KC_ESC),
    
    // Navigation/editing
    [COMBO_DF_TAB]         = COMBO(combo_df_tab, KC_TAB),
    [COMBO_SD_BSPC]        = COMBO(combo_sd_bspc, KC_BSPC),
    [COMBO_KL_ENT]         = COMBO(combo_kl_ent, KC_ENT),
    
    // Brackets
    [COMBO_ER_LPRN]        = COMBO(combo_er_lprn, KC_LPRN),
    [COMBO_UI_RPRN]        = COMBO(combo_ui_rprn, KC_RPRN),
    [COMBO_WE_LBRC]        = COMBO(combo_we_lbrc, KC_LBRC),
    [COMBO_IO_RBRC]        = COMBO(combo_io_rbrc, KC_RBRC),
    [COMBO_QW_LCBR]        = COMBO(combo_qw_lcbr, KC_LCBR),
    [COMBO_OP_RCBR]        = COMBO(combo_op_rcbr, KC_RCBR),
    
    // Programming operators - these need COMBO_ACTION for strings
    [COMBO_MCOMM_ARROW]    = COMBO_ACTION(combo_mcomm_arrow),
    [COMBO_DOTSLSH_DARROW] = COMBO_ACTION(combo_dotslsh_darrow),
    [COMBO_COMMM_DCLN]     = COMBO(combo_commm_dcln, KC_DCLN),  // If you have KC_DCLN defined
    
    // Caps Word
    [COMBO_SDF_CAPSWORD]   = COMBO(combo_sdf_capsword, CW_TOGG),
    
    // Layer access
    [COMBO_ZX_SYS]         = COMBO(combo_zx_sys, MO(_SYS)),
    [COMBO_SLASHDOT_MOUSE] = COMBO(combo_slashdot_mouse, MO(_MOUSE)),
    
    // Utility
    [COMBO_CV_PASTE]       = COMBO(combo_cv_paste, LCTL(KC_V)),
    [COMBO_XC_CUT]         = COMBO(combo_xc_cut, LCTL(KC_X)),
    [COMBO_ZA_UNDO]        = COMBO(combo_za_undo, LCTL(KC_Z)),
};

/* ==========================================================================
 * COMBO CALLBACKS
 * ==========================================================================
 */

// Process custom combo actions (for COMBO_ACTION entries)
void process_combo_event(uint16_t combo_index, bool pressed) {
    if (!pressed) return;  // Only act on press, not release
    
    switch (combo_index) {
        case COMBO_MCOMM_ARROW:
            SEND_STRING("->");
            break;
        case COMBO_DOTSLSH_DARROW:
            SEND_STRING("=>");  // or "::" for Odin scope
            break;
    }
}

// Make combos with mod-taps require tapping (not holding)
// This is critical for home row mods!
#ifdef COMBO_MUST_TAP_PER_COMBO
bool get_combo_must_tap(uint16_t combo_index, combo_t *combo) {
    // Check if any key in the combo is a mod-tap or layer-tap
    uint16_t key;
    uint8_t idx = 0;
    while ((key = pgm_read_word(&combo->keys[idx])) != COMBO_END) {
        switch (key) {
            case QK_MOD_TAP...QK_MOD_TAP_MAX:
            case QK_LAYER_TAP...QK_LAYER_TAP_MAX:
                return true;
        }
        idx += 1;
    }
    return false;
}
#endif

// Disable combos on certain layers
#ifdef COMBO_SHOULD_TRIGGER
bool combo_should_trigger(uint16_t combo_index, combo_t *combo, 
                          uint16_t keycode, keyrecord_t *record) {
    // Disable ALL combos on gaming layers
    if (layer_state_is(_GAMING) || layer_state_is(_ROGUELIKE)) {
        return false;
    }
    
    // Could add per-combo exceptions here
    // switch (combo_index) {
    //     case COMBO_JK_ESC:
    //         if (layer_state_is(_VIM)) return false;
    // }
    
    return true;
}
#endif

// Optional: Custom timing per combo
#ifdef COMBO_TERM_PER_COMBO
uint16_t get_combo_term(uint16_t combo_index, combo_t *combo) {
    switch (combo_index) {
        // 3-key combos need more time
        case COMBO_SDF_CAPSWORD:
            return 60;
        
        // Common combos can be faster
        case COMBO_JK_ESC:
        case COMBO_DF_TAB:
            return 35;
            
        default:
            return COMBO_TERM;
    }
}
#endif
```

### Step 2: Include in keyrecords.c

Your existing `keyrecords.c` should include this:

```c
/* keyrecords.c - Shared key records for QMK introspection
 * 
 * This file is #included (not compiled separately) in each keymap.c
 * so QMK's introspection can find the arrays.
 */

#include "naughtyusername.h"

// ===== COMBOS =====
#ifdef COMBO_ENABLE
#include "combos.h"
#endif

// ===== TAP DANCE =====
#ifdef TAP_DANCE_ENABLE
// ... your tap dance definitions
#endif

// ===== KEY OVERRIDES =====
#ifdef KEY_OVERRIDE_ENABLE  
// ... your key override definitions
#endif
```

### Step 3: Config.h Additions

Add to your userspace `config.h`:

```c
/* ===== COMBO CONFIGURATION ===== */
#define COMBO_TERM 40
#define COMBO_ONLY_FROM_LAYER _BASE
#define COMBO_MUST_TAP_PER_COMBO
#define COMBO_SHOULD_TRIGGER
// #define COMBO_TERM_PER_COMBO  // Uncomment if you want per-combo timing
```

### Step 4: Rules.mk

```makefile
COMBO_ENABLE = yes
```

---

## Quick Reference: Combo Macros

| Macro | Usage | When to Use |
|-------|-------|-------------|
| `COMBO(keys, KC)` | Simple keycode output | Most combos |
| `COMBO_ACTION(keys)` | Custom via `process_combo_event` | Strings, complex logic |

## Quick Reference: Timing

| Define | Effect |
|--------|--------|
| `COMBO_TERM 50` | Window to complete combo (default 50ms) |
| `COMBO_HOLD_TERM 150` | Window for must-hold combos |
| `COMBO_STRICT_TIMER` | Timer only on first key |
| `COMBO_NO_TIMER` | Fire on first release |

## Quick Reference: Behavior

| Define | Effect |
|--------|--------|
| `COMBO_MUST_HOLD_MODS` | Modifier combos require holding |
| `COMBO_MUST_PRESS_IN_ORDER` | Keys must be pressed in order |
| `COMBO_ONLY_FROM_LAYER N` | Always check from layer N |

---

## Tips for Your Setup

1. **Start small**: Add 3-5 combos, test thoroughly, then add more
2. **Use COMBO_MUST_TAP_PER_COMBO**: Essential with home row mods
3. **Disable on gaming**: Use `combo_should_trigger` to turn off combos in games
4. **Tune COMBO_TERM**: Start at 40ms, adjust based on your typing speed
5. **Horizontal combos**: Adjacent keys (JK, DF, UI) are easiest to hit
6. **Vertical combos**: Same finger different rows are harder - avoid
7. **3-key combos**: Good for rarely-used but important functions (Caps Word, layers)

---

## Example Combos for Odin Development

```c
// Odin-specific combos you might want:
COMBO(combo_dcln, KC_DCLN)           // :: (already in your custom keycodes)
COMBO_ACTION(combo_arrow)            // ->
COMBO_ACTION(combo_not_equal)        // !=
COMBO_ACTION(combo_less_equal)       // <=
COMBO_ACTION(combo_greater_equal)    // >=
COMBO_ACTION(combo_range)            // ..
COMBO_ACTION(combo_range_less)       // ..<

// In process_combo_event:
case COMBO_NOT_EQUAL:    SEND_STRING("!="); break;
case COMBO_LESS_EQUAL:   SEND_STRING("<="); break;
case COMBO_RANGE:        SEND_STRING(".."); break;
case COMBO_RANGE_LESS:   SEND_STRING("..<"); break;
```
