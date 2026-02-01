/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Kyria keymap using userspace wrappers
 *
 * This keymap uses the shared layouts from users/naughtyusername/wrappers.h
 * The only kyria-specific code here is LED handling, Rotary encoders, RGB
 */

#include "naughtyusername.h"

/* Include shared keyrecords (tap dance, combos, key overrides)
 * This MUST be included here (not compiled separately) for QMK's
 * introspection system to find the arrays.
 */
#include "keyrecords.c"

// clang-format off

/* ==========================================================================
 * KEYMAP LAYOUT
 * ==========================================================================
 * The Kyria physical layout is:
 *   - 3x6 main grid per side ( keys)
     - Third row has 2 extra thumb keys
 *   - 1x5 thumb cluster per side ( keys)
 *   - 50 keys total, 2 are rotary encoders
 *
 * The LAYOUT macro expects keys in this order:
 *   Row 1 Left (6), Row 1 Right (6),
 *   Row 2 Left (6), Row 2 Right (6),
 *   Row 3 Left (6+2) wrapper, Row 3 Right (6+2) wrapper
 *   Thumb Row 2 Left (5) Right (5) - written as one define
 */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_BASE] = LAYOUT_kyria_wrapper(
        ___BASE_L1_6___,                    ___BASE_R1_6___,
        ___BASE_L2_6___,                    ___BASE_R2_6___,
        ___BASE_L3_8___,                    ___BASE_R3_8___,
                         ___KYRIA_THUMB_BASE___
    ),

    /* LOWER - Numbers and Media */
    [_LOWER] = LAYOUT_kyria_wrapper(
        ___LOWER_L1_6___,                   ___LOWER_R1_6___,
        ___LOWER_L2_6___,                   ___LOWER_R2_6___,
        ___LOWER_L3_8___,                   ___LOWER_R3_8___,
                         ___KYRIA_THUMB_LOWER___
    ),

    /* RAISE - Symbols (Odin/Programming focused) */
    [_RAISE] = LAYOUT_kyria_wrapper(
        ___RAISE_L1_6___,                   ___RAISE_R1_6___,
        ___RAISE_L2_6___,                   ___RAISE_R2_6___,
        ___RAISE_L3_8___,                   ___RAISE_R3_8___,
                         ___KYRIA_THUMB_RAISE___
    ),

    /* FUNCTION - F-keys and Navigation */
    [_FUNCTION] = LAYOUT_kyria_wrapper(
        ___FUNC_L1_6___,                    ___FUNC_R1_6___,
        ___FUNC_L2_6___,                    ___FUNC_R2_6___,
        ___FUNC_L3_8___,                    ___FUNC_R3_8___,
                         ___KYRIA_THUMB_FUNC___
    ),

    /* ADJUST - System (Tri-layer: LOWER + RAISE) */
    [_ADJUST] = LAYOUT_kyria_wrapper(
        ___ADJUST_L1_6___,                  ___ADJUST_R1_6___,
        ___ADJUST_L2_6___,                  ___ADJUST_R2_6___,
        ___ADJUST_L3_8___,                  ___ADJUST_R3_8___,
                         ___KYRIA_THUMB_ADJUST___
    ),

    /* GAMING - No home row mods */
    [_GAMING] = LAYOUT_kyria_wrapper(
        ___GAMING_L1_6___,                  ___GAMING_R1_6___,
        ___GAMING_L2_6___,                  ___GAMING_R2_6___,
        ___GAMING_L3_8___,                  ___GAMING_R3_8___,
                         ___KYRIA_THUMB_GAMING___
    ),

    /* ROGUELIKE - Numpad for 8-directional movement */
    [_ROGUELIKE] = LAYOUT_kyria_wrapper(
        ___ROGUELIKE_L1_6___,               ___ROGUELIKE_R1_6___,
        ___ROGUELIKE_L2_6___,               ___ROGUELIKE_R2_6___,
        ___ROGUELIKE_L3_8___,               ___ROGUELIKE_R3_8___,
                         ___KYRIA_THUMB_ROGUELIKE___
    ),

    /* VIM - Plain QWERTY without home row mods */
    [_VIM] = LAYOUT_kyria_wrapper(
        ___VIM_L1_6___,                     ___VIM_R1_6___,
        ___VIM_L2_6___,                     ___VIM_R2_6___,
        ___VIM_L3_8___,                     ___VIM_R3_8___,
                         ___KYRIA_THUMB_VIM___
    ),

    /* SYS - System settings */
    [_SYS] = LAYOUT_kyria_wrapper(
        ___SYS_L1_6___,                     ___SYS_R1_6___,
        ___SYS_L2_6___,                     ___SYS_R2_6___,
        ___SYS_L3_8___,                     ___SYS_R3_8___,
                         ___KYRIA_THUMB_SYS___
    ),

    /* MOUSE - Mouse movement and buttons */
    [_MOUSE] = LAYOUT_kyria_wrapper(
        ___MOUSE_L1_6___,                   ___MOUSE_R1_6___,
        ___MOUSE_L2_6___,                   ___MOUSE_R2_6___,
        ___MOUSE_L3_8___,                   ___MOUSE_R3_8___,
                         ___KYRIA_THUMB_MOUSE___
    ),
};

/* ==========================================================================
 * KYRIA-SPECIFIC OVERRIDES
 * ==========================================================================
 * The _keymap functions let us add board-specific behavior while still
 * using the shared userspace code.
 */

#ifdef OLED_ENABLE
/* OLED Display - Left half only
 * We can customize this later with layer indicators, WPM, etc.
 */
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_180;  // Flip for slave side if needed
    }
    return rotation;
}

bool oled_task_user(void) {
    // Show current layer
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state)) {
        case _BASE:
            oled_write_ln_P(PSTR("Base"), false);
            break;
        case _LOWER:
            oled_write_ln_P(PSTR("Lower"), false);
            break;
        case _RAISE:
            oled_write_ln_P(PSTR("Raise"), false);
            break;
        case _FUNCTION:
            oled_write_ln_P(PSTR("Function"), false);
            break;
        case _ADJUST:
            oled_write_ln_P(PSTR("Adjust"), false);
            break;
        case _GAMING:
            oled_write_ln_P(PSTR("Gaming"), false);
            break;
        case _ROGUELIKE:
            oled_write_ln_P(PSTR("Roguelike"), false);
            break;
        case _VIM:
            oled_write_ln_P(PSTR("Vim"), false);
            break;
        case _SYS:
            oled_write_ln_P(PSTR("System"), false);
            break;
        case _MOUSE:
            oled_write_ln_P(PSTR("Mouse"), false);
            break;
        default:
            oled_write_ln_P(PSTR("???"), false);
    }

    // Show mods
    uint8_t mods = get_mods() | get_oneshot_mods();
    oled_write_P(PSTR("Mods: "), false);
    oled_write_P(mods & MOD_MASK_SHIFT ? PSTR("S") : PSTR(" "), false);
    oled_write_P(mods & MOD_MASK_CTRL  ? PSTR("C") : PSTR(" "), false);
    oled_write_P(mods & MOD_MASK_ALT   ? PSTR("A") : PSTR(" "), false);
    oled_write_ln_P(mods & MOD_MASK_GUI   ? PSTR("G") : PSTR(" "), false);

    // Show caps word status
    if (is_caps_word_on()) {
        oled_write_ln_P(PSTR("CAPS WORD"), false);
    }

    return false;
}
#endif
