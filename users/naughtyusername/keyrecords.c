/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * keyrecords.c - Tap dance, combo, and key override definitions
 *
 * IMPORTANT: This file is meant to be #included into keymap.c, NOT compiled
 * separately! QMK's introspection system requires these arrays to be in the
 * same compilation unit as the keymaps[] array.
 *
 * Usage in keymap.c:
 *   #include "naughtyusername.h"
 *   #include "keyrecords.c"  // <-- Include this AFTER the header
 *
 *   const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = { ... };
 */

// Guard against double inclusion and standalone compilation
#ifndef KEYRECORDS_INCLUDED
#define KEYRECORDS_INCLUDED

#ifdef COMBO_ENABLE
#include "combos.h"
#endif

/* ==========================================================================
 * TAP DANCE DEFINITIONS
 * ==========================================================================
 * The enum (TD_Q_ESC, etc.) is defined in naughtyusername.h
 */
#ifdef TAP_DANCE_ENABLE
tap_dance_action_t tap_dance_actions[] = {
    // Add more tap dances here
};
#endif

/* ==========================================================================
 * KEY OVERRIDES
 * ==========================================================================
 * Shift+Backspace = Delete
 * This is super handy - no dedicated Delete key needed.
 */
#ifdef KEY_OVERRIDE_ENABLE
const key_override_t shift_bspc_override =
    ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);

const key_override_t *key_overrides[] = {
    &shift_bspc_override,
    // Add more overrides here
    NULL // Null terminator required by QMK
};
#endif

#endif // KEYRECORDS_INCLUDED
