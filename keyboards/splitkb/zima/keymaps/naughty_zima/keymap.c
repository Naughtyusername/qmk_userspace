/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Splitkb Zima macropad keymap
 *
 * Layout: 4x3 ortholinear (12 keys) + rotary encoder
 * Hardware: OLED, RGB underglow, Piezo speaker
 *
 * This is a standalone keymap - no userspace includes to avoid
 * pulling in combos/tap dance that aren't defined here.
 */

#include QMK_KEYBOARD_H

enum zima_layers { _NUMPAD = 0, _MEDIA, _SYSTEM };

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* NUMPAD (Layer 0)
     * ┌──────┬──────┬──────┐
     * │ Mute │Media │System│  <- Layer toggles
     * ├──────┼──────┼──────┤
     * │  7   │  8   │  9   │
     * ├──────┼──────┼──────┤
     * │  4   │  5   │  6   │
     * ├──────┼──────┼──────┤
     * │  1   │  2   │  3   │
     * └──────┴──────┴──────┘
     * Encoder: Volume Up/Down
     */
    [_NUMPAD] = LAYOUT_ortho_4x3(
        KC_MUTE,   TG(_MEDIA),  TG(_SYSTEM),
        KC_P7,     KC_P8,       KC_P9,
        KC_P4,     KC_P5,       KC_P6,
        KC_P1,     KC_P2,       KC_P3
    ),

    /* MEDIA (Layer 1)
     * ┌──────┬──────┬──────┐
     * │ Mute │XXXXXX│System│  <- Back to numpad
     * ├──────┼──────┼──────┤
     * │ Prev │ Play │ Next │
     * ├──────┼──────┼──────┤
     * │      │ Stop │      │
     * ├──────┼──────┼──────┤
     * │      │      │      │
     * └──────┴──────┴──────┘
     * Encoder: Track Prev/Next
     */
    [_MEDIA] = LAYOUT_ortho_4x3(
        KC_MUTE,   _______,     TG(_SYSTEM),
        KC_MPRV,   KC_MPLY,     KC_MNXT,
        _______,   KC_MSTP,     _______,
        _______,   _______,     _______
    ),

    /* SYSTEM (Layer 2)
     * ┌──────┬──────┬──────┐
     * │RGBTog│      │XXXXXX│  <- Back to numpad
     * ├──────┼──────┼──────┤
     * │ Hue+ │ Sat+ │ Val+ │
     * ├──────┼──────┼──────┤
     * │ Hue- │ Sat- │ Val- │
     * ├──────┼──────┼──────┤
     * │ Boot │EEClr │RGBMod│
     * └──────┴──────┴──────┘
     * Encoder: RGB Mode cycle
     */
    [_SYSTEM] = LAYOUT_ortho_4x3(
        RM_TOGG,   _______,     _______,
        RM_HUEU,   RM_SATU,     RM_VALU,
        RM_HUED,   RM_SATD,     RM_VALD,
        QK_BOOT,   EE_CLR,      RM_NEXT
    )
};
// clang-format on

/* Rotary encoder mapping */
#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_NUMPAD] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [_MEDIA] = {ENCODER_CCW_CW(KC_MPRV, KC_MNXT)},
    [_SYSTEM] = {ENCODER_CCW_CW(RM_PREV, RM_NEXT)}};
#endif
