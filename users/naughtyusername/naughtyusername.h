/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * naughtyusername.h - Main userspace header
 *
 * This file is automatically included when building any keymap named
 * "naughtyusername". It defines all shared layers, keycodes, and macros
 * that are consistent across all keyboards.
 */

#pragma once

#include QMK_KEYBOARD_H

/* ==========================================================================
 * LAYER DEFINITIONS
 * ==========================================================================
 * These must be consistent across ALL keyboards. The tri-layer feature
 * depends on _LOWER, _RAISE, and _ADJUST being specific layer numbers.
 *
 * Note: We use an enum so the compiler assigns sequential numbers (0,1,2...)
 * and we can easily add/remove/reorder layers later.
 */
enum userspace_layers {
    _BASE = 0,
    _VIM,       // no home row mods, top row combo GACS mods tho.
    _LOWER,     // Numbers, numpad, media controls
    _RAISE,     // Symbols, programming operators
    _FUNCTION,  // F-keys, navigation
    _ADJUST,    // System controls, mouse (tri-layer: LOWER + RAISE)
    _GAMING,    // No home row mods, direct keys for gaming
    _GAMING2,   // Function keys and numbers.
    _ROGUELIKE, // numpad for the rl's that hate vim :(
    _SYS,   // bootloader and other shit that i dont like accessible. NKRO etc.
    _MOUSE, // For halcyon corne cirque trackpad
};

/* ==========================================================================
 * CUSTOM KEYCODES
 * ==========================================================================
 * These are handled in process_record_user() in naughtyusername.c
 *
 * IMPORTANT: We start from QK_USER (was SAFE_RANGE in older QMK) to avoid
 * conflicts with QMK's internal keycodes. If a keymap needs its own custom
 * keycodes, it should start from NEW_SAFE_RANGE defined below.
 */
enum userspace_keycodes {
    KC_ASSIGN = QK_USER, // :=  (Odin/Go assignment)
    KC_ARROP,            // ->  (Arrow operator)
    KC_DCLN,             // ::  (Double colon - C++/Odin)
    KC_RANGE,            // ..= (Odin range operator)
    KC_HMDR,             // ~/  (Home directory shortcut)

    NUMWORD, // Num Word - combo-activated numbers layer lock

    // Add more shared keycodes above this line
    NEW_SAFE_RANGE // Keymaps can use this for their own keycodes
};

/* ==========================================================================
 * HOME ROW MODS - GACS (Gui, Alt, Ctrl, Shift)
 * ==========================================================================
 * The "GACS" order means:
 *   - Pinky: GUI (Super/Win/Cmd)
 *   - Ring:  Alt
 *   - Middle: Ctrl
 *   - Index: Shift
 *
 * This is mirrored on both hands. The logic is that Shift is most commonly
 * used with the opposite hand (for capital letters), so it goes on the
 * strongest finger (index). GUI is least used, so it's on the weakest (pinky).
 */

// Left hand home row
#define HM_A LGUI_T(KC_A)
#define HM_S LALT_T(KC_S)
#define HM_D LCTL_T(KC_D)
#define HM_F LSFT_T(KC_F)

// Right hand home row (mirrored)
#define HM_J RSFT_T(KC_J)
#define HM_K RCTL_T(KC_K)
#define HM_L RALT_T(KC_L)
#define HM_SCLN RGUI_T(KC_SCLN)

/* ==========================================================================
 * LAYER TAP SHORTCUTS
 * ==========================================================================
 * LT(layer, keycode) = tap for keycode, hold for layer
 * These make the keymap definitions much more readable.
 */
#define LOW MO(_LOWER)
#define RAI MO(_RAISE)
#define FUN MO(_FUNCTION)
#define ADJ MO(_ADJUST)
#define GAM TG(_GAMING)

// Thumb keys with layer-tap
#define SP_RAI LT(_RAISE, KC_SPC) // Space on tap, Raise on hold
#define SP_LOW                                                                 \
    LT(_LOWER, KC_SPC) // Space on tap, Lower on hold (for use on Raise layer)
#define ENT_LOW LT(_LOWER, KC_ENT)    // Enter on tap, Lower on hold
#define FUN_TAB LT(_FUNCTION, KC_TAB) // Tab on tap, Func on hold

/* ==========================================================================
 * TAP DANCE DECLARATIONS
 * ==========================================================================
 * The actual tap_dance_actions[] array is defined in naughtyusername.c
 * We just declare the enum here so keymaps can reference TD(TD_Q_ESC) etc.
 */

/* ==========================================================================
 * WRAPPER INCLUDES
 * ==========================================================================
 * The wrappers.h file contains all the LAYOUT wrapper macros.
 * We include it here so any file that includes naughtyusername.h
 * automatically gets access to the wrapper macros.
 *
 * The __ASSEMBLER__ guard prevents issues when this header is processed
 * by the assembler (which doesn't understand C code).
 */
#ifndef __ASSEMBLER__
#include "wrappers.h"
#endif

/* ==========================================================================
 * COMBO DECLARATIONS
 * ==========================================================================
 * QMK's introspection system needs to see these declarations.
 * The actual definitions are in naughtyusername.c
 */
#ifdef COMBO_ENABLE
extern combo_t key_combos[];
extern uint16_t COMBO_LEN;
#endif

/* ==========================================================================
 * TAP DANCE DECLARATIONS
 * ==========================================================================
 * QMK's introspection system needs to see these declarations.
 * The actual definitions are in naughtyusername.c
 */
#ifdef TAP_DANCE_ENABLE
extern tap_dance_action_t tap_dance_actions[];
#endif

/* ==========================================================================
 * KEY OVERRIDE DECLARATIONS
 * ==========================================================================
 * QMK's introspection system needs to see these declarations.
 * The actual definitions are in naughtyusername.c
 */
#ifdef KEY_OVERRIDE_ENABLE
extern const key_override_t *key_overrides[];
#endif

/* ==========================================================================
 * FUNCTION DECLARATIONS
 * ==========================================================================
 * These are the "_keymap" versions of QMK callbacks. Your userspace defines
 * the "_user" version and calls these. Keymaps can optionally override them.
 *
 * The __attribute__((weak)) in the .c file means: "use this default
 * implementation unless someone else defines a stronger one"
 */
bool process_record_keymap(uint16_t keycode, keyrecord_t *record);
layer_state_t layer_state_set_keymap(layer_state_t state);
void matrix_scan_keymap(void);
void keyboard_post_init_keymap(void);
