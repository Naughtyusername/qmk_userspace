/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Planck EZ keymap using userspace wrappers
 *
 * Layout: 4x12 ortholinear with 2u centered spacebar (47 keys)
 * Hardware: RGB Matrix, Piezo speaker
 */

#include QMK_KEYBOARD_H
#include "naughtyusername.h"

// Include combos, tap dance, key overrides (introspection needs these here)
#include "keyrecords.c"

/* ==========================================================================
 * PLANCK-SPECIFIC: Audio
 * ==========================================================================
 */
#ifdef AUDIO_ENABLE
float combo_song[][2] = SONG(QWERTY_SOUND);
float gaming_on_song[][2] = SONG(STARTUP_SOUND);
float gaming_off_song[][2] = SONG(GOODBYE_SOUND);
float roguelike_on_song[][2] = SONG(ZELDA_PUZZLE);
float roguelike_off_song[][2] = SONG(ZELDA_TREASURE);
float mouse_on_song[][2] = SONG(TERMINAL_SOUND);

float my_startup_song[][2] = SONG(E__NOTE(_E5), E__NOTE(_G5), E__NOTE(_E6),
                                  E__NOTE(_C6), E__NOTE(_D6), E__NOTE(_G6));
#endif

void keyboard_post_init_keymap(void) {
#ifdef AUDIO_ENABLE
    PLAY_SONG(my_startup_song);
#endif
}

/* ==========================================================================
 * KEYMAPS
 * ==========================================================================
 * Using 6-column wrapper macros for the 3 alpha rows (12 cols = 6L + 6R)
 * Plus Planck-specific bottom row macros (11 keys with 2u space)
 *
 * Planck physical layout:
 * ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
 * │ L1  │ L1  │ L1  │ L1  │ L1  │ L1  │ R1  │ R1  │ R1  │ R1  │ R1  │ R1  │
 * ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
 * │ L2  │ L2  │ L2  │ L2  │ L2  │ L2  │ R2  │ R2  │ R2  │ R2  │ R2  │ R2  │
 * ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
 * │ L3  │ L3  │ L3  │ L3  │ L3  │ L3  │ R3  │ R3  │ R3  │ R3  │ R3  │ R3  │
 * ├─────┼─────┼─────┼─────┼─────┼─────┴─────┼─────┼─────┼─────┼─────┼─────┤
 * │ B1  │ B2  │ B3  │ B4  │ B5  │   SPACE   │ B6  │ B7  │ B8  │ B9  │ B10 │
 * └─────┴─────┴─────┴─────┴─────┴───────────┴─────┴─────┴─────┴─────┴─────┘
 */

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* BASE - QWERTY with Home Row Mods */
    [_BASE] = LAYOUT_planck_wrapper(
        ___BASE_L1_6___,                    ___BASE_R1_6___,
        ___BASE_L2_6___,                    ___BASE_R2_6___,
        ___BASE_L3_6___,                    ___BASE_R3_6___,
                      ___PLANCK_BOTTOM_BASE___
    ),

    /* LOWER - Numbers and Media */
    [_LOWER] = LAYOUT_planck_wrapper(
        ___LOWER_L1_6___,                   ___LOWER_R1_6___,
        ___LOWER_L2_6___,                   ___LOWER_R2_6___,
        ___LOWER_L3_6___,                   ___LOWER_R3_6___,
                      ___PLANCK_BOTTOM_LOWER___
    ),

    /* RAISE - Symbols (Odin/Programming focused) */
    [_RAISE] = LAYOUT_planck_wrapper(
        ___RAISE_L1_6___,                   ___RAISE_R1_6___,
        ___RAISE_L2_6___,                   ___RAISE_R2_6___,
        ___RAISE_L3_6___,                   ___RAISE_R3_6___,
                      ___PLANCK_BOTTOM_RAISE___
    ),

    /* FUNCTION - F-keys and Navigation */
    [_FUNCTION] = LAYOUT_planck_wrapper(
        ___FUNC_L1_6___,                    ___FUNC_R1_6___,
        ___FUNC_L2_6___,                    ___FUNC_R2_6___,
        ___FUNC_L3_6___,                    ___FUNC_R3_6___,
                      ___PLANCK_BOTTOM_FUNC___
    ),

    /* ADJUST - System (Tri-layer: LOWER + RAISE) */
    [_ADJUST] = LAYOUT_planck_wrapper(
        ___ADJUST_L1_6___,                  ___ADJUST_R1_6___,
        ___ADJUST_L2_6___,                  ___ADJUST_R2_6___,
        ___ADJUST_L3_6___,                  ___ADJUST_R3_6___,
                      ___PLANCK_BOTTOM_ADJUST___
    ),

    /* GAMING - No home row mods */
    [_GAMING] = LAYOUT_planck_wrapper(
        ___GAMING_L1_6___,                  ___GAMING_R1_6___,
        ___GAMING_L2_6___,                  ___GAMING_R2_6___,
        ___GAMING_L3_6___,                  ___GAMING_R3_6___,
                      ___PLANCK_BOTTOM_GAMING___
    ),

    /* ROGUELIKE - Numpad for 8-directional movement */
    [_ROGUELIKE] = LAYOUT_planck_wrapper(
        ___ROGUELIKE_L1_6___,               ___ROGUELIKE_R1_6___,
        ___ROGUELIKE_L2_6___,               ___ROGUELIKE_R2_6___,
        ___ROGUELIKE_L3_6___,               ___ROGUELIKE_R3_6___,
                      ___PLANCK_BOTTOM_ROGUELIKE___
    ),

    /* VIM - Plain QWERTY without home row mods */
    [_VIM] = LAYOUT_planck_wrapper(
        ___VIM_L1_6___,                     ___VIM_R1_6___,
        ___VIM_L2_6___,                     ___VIM_R2_6___,
        ___VIM_L3_6___,                     ___VIM_R3_6___,
                      ___PLANCK_BOTTOM_VIM___
    ),

    /* SYS - System settings */
    [_SYS] = LAYOUT_planck_wrapper(
        ___SYS_L1_6___,                     ___SYS_R1_6___,
        ___SYS_L2_6___,                     ___SYS_R2_6___,
        ___SYS_L3_6___,                     ___SYS_R3_6___,
                      ___PLANCK_BOTTOM_SYS___
    ),

    /* MOUSE - Mouse movement and buttons */
    [_MOUSE] = LAYOUT_planck_wrapper(
        ___MOUSE_L1_6___,                   ___MOUSE_R1_6___,
        ___MOUSE_L2_6___,                   ___MOUSE_R2_6___,
        ___MOUSE_L3_6___,                   ___MOUSE_R3_6___,
                      ___PLANCK_BOTTOM_MOUSE___
    ),

};
// clang-format on

/* ==========================================================================
 * PLANCK-SPECIFIC: Combo sound
 * ==========================================================================
 */
// void process_combo_event(uint16_t combo_index, bool pressed) {
//     if (pressed && combo_index == 0) { // JK escape combo
// #ifdef AUDIO_ENABLE
//         PLAY_SONG(combo_song);
// #endif
//     }
// }

/* ==========================================================================
 * PLANCK-SPECIFIC: Layer state with audio feedback
 * ==========================================================================
 * This overrides the weak layer_state_set_keymap() from naughtyusername.c
 */
layer_state_t layer_state_set_keymap(layer_state_t state) {
#ifdef AUDIO_ENABLE
    static bool gaming_was_on = false;
    static bool roguelike_was_on = false;
    static bool mouse_was_on = false;

    bool gaming_is_on = layer_state_cmp(state, _GAMING);
    bool roguelike_is_on = layer_state_cmp(state, _ROGUELIKE);
    bool mouse_is_on = layer_state_cmp(state, _MOUSE);

    // Gaming layer audio
    if (gaming_is_on && !gaming_was_on) {
        PLAY_SONG(gaming_on_song);
    } else if (!gaming_is_on && gaming_was_on) {
        PLAY_SONG(gaming_off_song);
    }
    gaming_was_on = gaming_is_on;

    // Roguelike layer audio
    if (roguelike_is_on && !roguelike_was_on) {
        PLAY_SONG(roguelike_on_song);
    } else if (!roguelike_is_on && roguelike_was_on) {
        PLAY_SONG(roguelike_off_song);
    }
    roguelike_was_on = roguelike_is_on;

    // Mouse layer audio
    if (mouse_is_on && !mouse_was_on) {
        PLAY_SONG(mouse_on_song);
    }
    mouse_was_on = mouse_is_on;
#endif

    return state;
}

/* ==========================================================================
 * PLANCK-SPECIFIC: RGB Matrix layer indication
 * ==========================================================================
 */
#ifdef RGB_MATRIX_ENABLE
bool rgb_matrix_indicators_user(void) {
    switch (get_highest_layer(layer_state)) {
    case _LOWER:
        rgb_matrix_set_color_all(0, 0, 255); // Blue
        break;
    case _RAISE:
        rgb_matrix_set_color_all(0, 255, 0); // Green
        break;
    case _FUNCTION:
        rgb_matrix_set_color_all(255, 255, 0); // Yellow
        break;
    case _ADJUST:
        rgb_matrix_set_color_all(191, 0, 255); // Purple
        break;
    case _GAMING:
        rgb_matrix_set_color_all(255, 0, 0); // Red
        break;
    case _ROGUELIKE:
        rgb_matrix_set_color_all(255, 128, 0); // Orange
        break;
    case _VIM:
        rgb_matrix_set_color_all(0, 255, 128); // Teal
        break;
    case _SYS:
        rgb_matrix_set_color_all(255, 0, 255); // Magenta
        break;
    case _MOUSE:
        rgb_matrix_set_color_all(0, 255, 255); // Cyan
        break;
    default:
        return true; // Use default RGB effect
    }
    return false;
}
#endif
