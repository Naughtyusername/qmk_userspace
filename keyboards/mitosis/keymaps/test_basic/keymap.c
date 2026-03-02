/* Bare-bones Mitosis keymap for drop testing.
 * No userspace, no HRMs, no tap-hold, no combos.
 * Same key positions as naughtyusername base layer.
 * If keys still drop here, it's the hardware.
 */

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
        // Row 1: top alpha
        KC_Q, KC_W, KC_E, KC_R, KC_T,       KC_Y, KC_U, KC_I, KC_O, KC_P,
        // Row 2: home row - plain, no HRMs
        KC_A, KC_S, KC_D, KC_F, KC_G,       KC_H, KC_J, KC_K, KC_L, KC_SCLN,
        // Row 3: bottom alpha
        KC_Z, KC_X, KC_C, KC_V, KC_B,       KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH,
        // Thumb row 1 (upper): Shft Ctrl Alt  Gui     Gui  Alt  Ctrl Shft
        KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI, KC_RGUI, KC_RALT, KC_RCTL, KC_RSFT,
        // Thumb row 2 (lower): Esc  ___  Bspc Spc     Ent  Tab  ___  ___
        KC_ESC, KC_NO, KC_BSPC, KC_SPC,     KC_ENT, KC_TAB, KC_NO, KC_NO
    ),
};
