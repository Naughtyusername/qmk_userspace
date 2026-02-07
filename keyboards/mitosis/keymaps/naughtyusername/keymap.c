/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Mitosis keymap using userspace wrappers
 *
 * This keymap uses the shared layouts from users/naughtyusername/wrappers.h
 * The only Mitosis-specific code here is the LED handling.
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
 * The Mitosis physical layout is:
 *   - 3x5 main grid per side (30 keys)
 *   - 2x4 thumb cluster per side (16 keys)
 *   - 46 keys total
 *
 * The LAYOUT macro expects keys in this order:
 *   Row 1 Left (5), Row 1 Right (5),
 *   Row 2 Left (5), Row 2 Right (5),
 *   Row 3 Left (5), Row 3 Right (5),
 *   Thumb Row 1 Left (4), Thumb Row 1 Right (4),
 *   Thumb Row 2 Left (4), Thumb Row 2 Right (4)
 */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* BASE - QWERTY with Home Row Mods (GACS)
     * See wrappers.h for the actual key definitions
     */
    [_BASE] = LAYOUT_mitosis_wrapper(
        ___BASE_L1___,                      ___BASE_R1___,
        ___BASE_L2___,                      ___BASE_R2___,
        ___BASE_L3___,                      ___BASE_R3___,
        ___MITOSIS_THUMB_BASE_L1___,        ___MITOSIS_THUMB_BASE_R1___,
        ___MITOSIS_THUMB_BASE_L2___,        ___MITOSIS_THUMB_BASE_R2___
    ),

    /* VIM - Plain QWERTY without home row mods */
    [_VIM] = LAYOUT_mitosis_wrapper(
        ___VIM_L1___,                       ___VIM_R1___,
        ___VIM_L2___,                       ___VIM_R2___,
        ___VIM_L3___,                       ___VIM_R3___,
        ___MITOSIS_THUMB_VIM_L1___,         ___MITOSIS_THUMB_VIM_R1___,
        ___MITOSIS_THUMB_VIM_L2___,         ___MITOSIS_THUMB_VIM_R2___
    ),

    /* LOWER - Numbers and Media */
    [_LOWER] = LAYOUT_mitosis_wrapper(
        ___LOWER_L1___,                     ___LOWER_R1___,
        ___LOWER_L2___,                     ___LOWER_R2___,
        ___LOWER_L3___,                     ___LOWER_R3___,
        ___MITOSIS_THUMB_LOWER_L1___,       ___MITOSIS_THUMB_LOWER_R1___,
        ___MITOSIS_THUMB_LOWER_L2___,       ___MITOSIS_THUMB_LOWER_R2___
    ),

    /* RAISE - Symbols */
    [_RAISE] = LAYOUT_mitosis_wrapper(
        ___RAISE_L1___,                     ___RAISE_R1___,
        ___RAISE_L2___,                     ___RAISE_R2___,
        ___RAISE_L3___,                     ___RAISE_R3___,
        ___MITOSIS_THUMB_RAISE_L1___,       ___MITOSIS_THUMB_RAISE_R1___,
        ___MITOSIS_THUMB_RAISE_L2___,       ___MITOSIS_THUMB_RAISE_R2___
    ),

    /* FUNCTION - F-keys and Navigation */
    [_FUNCTION] = LAYOUT_mitosis_wrapper(
        ___FUNC_L1___,                      ___FUNC_R1___,
        ___FUNC_L2___,                      ___FUNC_R2___,
        ___FUNC_L3___,                      ___FUNC_R3___,
        ___MITOSIS_THUMB_FUNC_L1___,        ___MITOSIS_THUMB_FUNC_R1___,
        ___MITOSIS_THUMB_FUNC_L2___,        ___MITOSIS_THUMB_FUNC_R2___
    ),

    /* ADJUST - System (Tri-layer: LOWER + RAISE) */
    [_ADJUST] = LAYOUT_mitosis_wrapper(
        ___ADJUST_L1___,                    ___ADJUST_R1___,
        ___ADJUST_L2___,                    ___ADJUST_R2___,
        ___ADJUST_L3___,                    ___ADJUST_R3___,
        ___MITOSIS_THUMB_ADJUST_L1___,      ___MITOSIS_THUMB_ADJUST_R1___,
        ___MITOSIS_THUMB_ADJUST_L2___,      ___MITOSIS_THUMB_ADJUST_R2___
    ),

    /* GAMING - No home row mods */
    [_GAMING] = LAYOUT_mitosis_wrapper(
        ___GAMING_L1___,                    ___GAMING_R1___,
        ___GAMING_L2___,                    ___GAMING_R2___,
        ___GAMING_L3___,                    ___GAMING_R3___,
        ___MITOSIS_THUMB_GAMING_L1___,      ___MITOSIS_THUMB_GAMING_R1___,
        ___MITOSIS_THUMB_GAMING_L2___,      ___MITOSIS_THUMB_GAMING_R2___
    ),

    /* GAMING2 - No home row mods */
    [_GAMING2] = LAYOUT_mitosis_wrapper(
        ___GAMING2_L1___,                    ___GAMING2_R1___,
        ___GAMING2_L2___,                    ___GAMING2_R2___,
        ___GAMING2_L3___,                    ___GAMING2_R3___,
        ___MITOSIS_THUMB_GAMING2_L1___,      ___MITOSIS_THUMB_GAMING2_R1___,
        ___MITOSIS_THUMB_GAMING2_L2___,      ___MITOSIS_THUMB_GAMING2_R2___
    ),

    /* ROGUELIKE - Numpad for 8-directional movement */
    [_ROGUELIKE] = LAYOUT_mitosis_wrapper(
        ___ROGUELIKE_L1___,                 ___ROGUELIKE_R1___,
        ___ROGUELIKE_L2___,                 ___ROGUELIKE_R2___,
        ___ROGUELIKE_L3___,                 ___ROGUELIKE_R3___,
        ___MITOSIS_THUMB_ROGUELIKE_L1___,   ___MITOSIS_THUMB_ROGUELIKE_R1___,
        ___MITOSIS_THUMB_ROGUELIKE_L2___,   ___MITOSIS_THUMB_ROGUELIKE_R2___
    ),

    /* SYS - System settings */
    [_SYS] = LAYOUT_mitosis_wrapper(
        ___SYS_L1___,                       ___SYS_R1___,
        ___SYS_L2___,                       ___SYS_R2___,
        ___SYS_L3___,                       ___SYS_R3___,
        ___MITOSIS_THUMB_SYS_L1___,         ___MITOSIS_THUMB_SYS_R1___,
        ___MITOSIS_THUMB_SYS_L2___,         ___MITOSIS_THUMB_SYS_R2___
    ),

    /* MOUSE - Mouse movement and buttons */
    [_MOUSE] = LAYOUT_mitosis_wrapper(
        ___MOUSE_L1___,                     ___MOUSE_R1___,
        ___MOUSE_L2___,                     ___MOUSE_R2___,
        ___MOUSE_L3___,                     ___MOUSE_R3___,
        ___MITOSIS_THUMB_MOUSE_L1___,       ___MITOSIS_THUMB_MOUSE_R1___,
        ___MITOSIS_THUMB_MOUSE_L2___,       ___MITOSIS_THUMB_MOUSE_R2___
    ),
};

// clang-format on

/* ==========================================================================
 * MITOSIS-SPECIFIC: LED Layer Indication
 * ==========================================================================
 * The Mitosis receiver has an RGB LED. We use it to show the current layer.
 * This overrides the weak layer_state_set_keymap() from naughtyusername.c
 *
 * LED color macros are defined in the Mitosis keyboard files:
 *   set_led_off, set_led_red, set_led_blue, set_led_green,
 *   set_led_yellow, set_led_magenta, set_led_cyan, set_led_white
 */
layer_state_t layer_state_set_keymap(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);

    switch (layer) {
    case _BASE:
        set_led_blue;
        break;
    case _LOWER:
        set_led_cyan;
        break;
    case _RAISE:
        set_led_yellow;
        break;
    case _FUNCTION:
        set_led_magenta;
        break;
    case _ADJUST:
        set_led_green;
        break;
    case _GAMING:
        set_led_white;
        break;
    case _GAMING2:
        set_led_white;
        break;
    case _ROGUELIKE:
        set_led_red;
        break;
    case _VIM:
        set_led_green;
        break;
    case _SYS:
        set_led_magenta;
        break;
    case _MOUSE:
        set_led_cyan;
        break;
    default:
        set_led_off;
        break;
    }

    return state;
}
