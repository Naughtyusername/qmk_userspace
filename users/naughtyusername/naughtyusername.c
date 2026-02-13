/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * naughtyusername.c - Main userspace source file
 *
 * This file contains all shared functionality that applies to every keyboard.
 * Individual keymaps can override the _keymap functions if they need
 * board-specific behavior.
 *
 * IMPORTANT: Tap dances, combos, and key overrides are in keyrecords.c
 * which gets #included into each keymap.c file for QMK introspection to work.

QMK has two different ways code gets into your firmware:

SRC += in rules.mk → Compiled as a separate file, linked together at the end.
Good for functions like process_record_user(), get_tapping_term(), etc. #include
in keymap.c → Preprocessor literally copy-pastes the content into keymap.c
before compilation. Required for arrays that QMK's introspection needs to find
(key_combos[], tap_dance_actions[], key_overrides[], and keymaps[]).
 */

#include "naughtyusername.h"
#include "numword.h"

/* ==========================================================================
 * TAPPING TERM PER KEY
 * ==========================================================================
 * Different fingers have different strengths and speeds.
 * Pinkies are slower, so they get a longer tapping term.
 */
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    // Pinkies get longer tapping term (weaker fingers)
    case HM_A:
    case HM_SCLN:
        return 185;

    // All other home row mods - consistent 200ms timing
    case HM_S:
    case HM_D:
    case HM_F:
    case HM_J:
    case HM_K:
    case HM_L:
        return 185;
    default:
        return TAPPING_TERM; // Use default from config.h
    }
}

/* ==========================================================================
 * CHORDAL HOLD CONFIGURATION
 * ==========================================================================
 * Chordal hold is a QMK feature that helps with home row mods.
 * It prevents accidental mod activation when rolling keys quickly.
 *
 * We override it here to allow SP_RAI and ENT_LOW to work on the same hand
 * (otherwise holding Space+another key on the same hand wouldn't trigger
 * Raise).
 */
bool get_chordal_hold(uint16_t tap_hold_keycode, keyrecord_t *tap_hold_record,
                      uint16_t other_keycode, keyrecord_t *other_record) {
    switch (tap_hold_keycode) {
    // Allow these to trigger hold even with same-hand keys
    case SP_RAI:
    case ENT_LOW:
    case SP_LOW:
        return true;
    }
    // Use default chordal hold behavior for everything else
    return get_chordal_hold_default(tap_hold_record, other_record);
}

/* ==========================================================================
 * FLOW TAP CONFIGURATION
 * ==========================================================================
 * Flow Tap helps with fast typing on home row mods. When you type quickly,
 * it registers taps instead of holds, preventing "thE" when you meant "the".
 *
 * We only enable it for alpha keys and common punctuation.
 */
bool is_flow_tap_key(uint16_t keycode) {
    // Disable Flow Tap when using modifier hotkeys
    if ((get_mods() & (MOD_MASK_CG | MOD_BIT_LALT)) != 0) {
        return false;
    }

    switch (get_tap_keycode(keycode)) {
    case KC_A ... KC_Z:
    case KC_DOT:
    case KC_COMM:
    case KC_SCLN:
    case KC_SLSH:
        return true;
    }
    return false;
}

uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record,
                           uint16_t prev_keycode) {
    if (is_flow_tap_key(keycode) && is_flow_tap_key(prev_keycode)) {
        return FLOW_TAP_TERM; // Defined in config.h
    }
    return 0; // Disable Flow Tap for non-alpha keys
}

/* ==========================================================================
 * WEAK KEYMAP FUNCTIONS
 * ==========================================================================
 * These are the defaults. If a keymap defines its own version, that takes
 * precedence. The __attribute__((weak)) tells the linker "use this unless
 * there's a stronger definition elsewhere."
 */

__attribute__((weak)) bool process_record_keymap(uint16_t keycode,
                                                 keyrecord_t *record) {
    return true; // Continue processing
}

__attribute__((weak)) layer_state_t
layer_state_set_keymap(layer_state_t state) {
    return state; // No modification
}

__attribute__((weak)) void matrix_scan_keymap(void) {
    // Nothing by default
}

__attribute__((weak)) void keyboard_post_init_keymap(void) {
    // Nothing by default
}

/* ==========================================================================
 * PROCESS RECORD USER
 * ==========================================================================
 * This handles all custom keycodes. It's called for EVERY keypress.
 *
 * The flow is:
 *   1. Check if it's one of our custom keycodes
 *   2. If yes, handle it and return false (stop further processing)
 *   3. If no, call process_record_keymap() for board-specific handling
 *   4. Return true to continue normal processing
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_num_word(keycode, record)) {
        return false;
    }

    // Only act on key press, not release
    if (record->event.pressed) {
        switch (keycode) {
        // Double colon :: (C++, Odin, Rust)
        case KC_DCLN:
            tap_code16(KC_COLON);
            tap_code16(KC_COLON);
            return false;
        }
    }

    // Pass to keymap-specific handler, then continue normal processing
    return process_record_keymap(keycode, record);
}

/* ==========================================================================
 * LAYER STATE SET USER
 * ==========================================================================
 * Called whenever the layer state changes. We use this for:
 *   1. Tri-layer (LOWER + RAISE = ADJUST)
 *   2. Any layer-based lighting/effects
 *   3. Keymap-specific layer handling
 */
layer_state_t layer_state_set_user(layer_state_t state) {
    // Update tri-layer state
    state = update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);

    // Call keymap-specific handler (e.g., for LED colors)
    return layer_state_set_keymap(state);
}

/* ==========================================================================
 * MATRIX SCAN USER
 * ==========================================================================
 * Called every matrix scan cycle (very frequently).
 * Keep this FAST - don't put slow code here!
 *
 * We just delegate to the keymap version for board-specific stuff
 * (like the Mitosis LED handling).
 */
void matrix_scan_user(void) { matrix_scan_keymap(); }

/* ==========================================================================
 * KEYBOARD POST INIT USER
 * ==========================================================================
 * Called once after the keyboard initializes. Good for setting up
 * default states, RGB modes, etc.
 */
void keyboard_post_init_user(void) { keyboard_post_init_keymap(); }
