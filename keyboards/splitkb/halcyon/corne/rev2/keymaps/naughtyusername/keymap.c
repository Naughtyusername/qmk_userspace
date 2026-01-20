/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Halcyon Corne keymap using userspace wrappers
 *
 * Layout: 3x6 + 3 thumbs per side = 42 keys total
 * Hardware: OLED on left, Cirque trackpad on right
 */

// clang-format off
#include QMK_KEYBOARD_H
#include "naughtyusername.h"

#ifdef HLC_TFT_DISPLAY
#include "splitkb/hlc_tft_display/hlc_tft_display.h"
#endif

// Include combos, tap dance, key overrides (introspection needs these here)
#include "keyrecords.c"

/* ==========================================================================
 * KEYMAPS
 * ==========================================================================
 * Using 6-column wrapper macros (___LAYER_L1_6___, etc.) which expand
 * the 5-column core with outer columns for the Corne's extra keys.
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* BASE - QWERTY with Home Row Mods
     * ┌─────┬─────┬─────┬─────┬─────┬─────┐       ┌─────┬─────┬─────┬─────┬─────┬─────┐
     * │ Tab │ESC/Q│  W  │  E  │  R  │  T  │       │  Y  │  U  │  I  │  O  │  P  │ Bsp │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │ Esc │A/Gui│S/Alt│D/Ctl│F/Sft│  G  │       │  H  │J/Sft│K/Ctl│L/Alt│;/Gui│  '  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │Shift│  Z  │  X  │  C  │  V  │  B  │       │  N  │  M  │  ,  │  .  │  /  │Shift│
     * └─────┴─────┴─────┼─────┼─────┼─────┤       ├─────┼─────┼─────┼─────┴─────┴─────┘
     *        TODO       │     │ Bsp │Sp/Ra│       │En/Lo│     │     │
     *                   └─────┴─────┴─────┘       └─────┴─────┴─────┘
     */
    [_BASE] = LAYOUT_corne_wrapper(
        ___BASE_L1_6___,                    ___BASE_R1_6___,
        ___BASE_L2_6___,                    ___BASE_R2_6___,
        ___BASE_L3_6___,                    ___BASE_R3_6___,
                         ___CORNE_THUMB_BASE___
    ),

    /* LOWER - Numbers and Media */
    [_LOWER] = LAYOUT_corne_wrapper(
        ___LOWER_L1_6___,                   ___LOWER_R1_6___,
        ___LOWER_L2_6___,                   ___LOWER_R2_6___,
        ___LOWER_L3_6___,                   ___LOWER_R3_6___,
                         ___CORNE_THUMB_LOWER___
    ),

    /* RAISE - Symbols (Odin/Programming focused) */
    [_RAISE] = LAYOUT_corne_wrapper(
        ___RAISE_L1_6___,                   ___RAISE_R1_6___,
        ___RAISE_L2_6___,                   ___RAISE_R2_6___,
        ___RAISE_L3_6___,                   ___RAISE_R3_6___,
                         ___CORNE_THUMB_RAISE___
    ),

    /* FUNCTION - F-keys and Navigation */
    [_FUNCTION] = LAYOUT_corne_wrapper(
        ___FUNC_L1_6___,                    ___FUNC_R1_6___,
        ___FUNC_L2_6___,                    ___FUNC_R2_6___,
        ___FUNC_L3_6___,                    ___FUNC_R3_6___,
                         ___CORNE_THUMB_FUNC___
    ),

    /* ADJUST - System (Tri-layer: LOWER + RAISE) */
    [_ADJUST] = LAYOUT_corne_wrapper(
        ___ADJUST_L1_6___,                  ___ADJUST_R1_6___,
        ___ADJUST_L2_6___,                  ___ADJUST_R2_6___,
        ___ADJUST_L3_6___,                  ___ADJUST_R3_6___,
                         ___CORNE_THUMB_ADJUST___
    ),

    /* GAMING - No home row mods */
    [_GAMING] = LAYOUT_corne_wrapper(
        ___GAMING_L1_6___,                  ___GAMING_R1_6___,
        ___GAMING_L2_6___,                  ___GAMING_R2_6___,
        ___GAMING_L3_6___,                  ___GAMING_R3_6___,
                         ___CORNE_THUMB_GAMING___
    ),
};

/* ==========================================================================
 * CORNE-SPECIFIC OVERRIDES
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

#ifdef POINTING_DEVICE_ENABLE
/* Cirque Trackpad - Right half
 * This runs after the shared userspace pointing device code (if any).
 * We can add Corne-specific pointing behavior here.
 */

// Optional: Custom pointing device behavior
// void pointing_device_init_keymap(void) {
//     // Corne-specific trackpad init
// }

// Optional: Modify reports before sending
// report_mouse_t pointing_device_task_keymap(report_mouse_t mouse_report) {
//     // Corne-specific modifications
//     return mouse_report;
// }
#endif
