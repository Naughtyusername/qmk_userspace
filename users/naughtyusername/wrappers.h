/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * wrappers.h - Layout wrapper macros
 *
 * This file defines reusable layout "building blocks" that can be combined
 * differently for each keyboard. The idea is:
 *
 *   1. Define your CORE layout (the 3x5 alpha grid) once
 *   2. Define keyboard-specific "wrappers" that add thumb keys
 *   3. Each keymap.c just assembles the pieces
 *
 * HOW IT WORKS:
 * -------------
 * C preprocessor macros are expanded in multiple passes. This lets us do:
 *
 *   LAYOUT_mitosis_wrapper(
 *       ___BASE_L1___, ___BASE_R1___,   // Expands to: KC_Q, KC_W, ...
 *       ...
 *   )
 *
 * The wrapper macro passes everything to the real LAYOUT() macro.
 */

#pragma once

/* ==========================================================================
 * LAYOUT WRAPPER MACROS
 * ==========================================================================
 * Each keyboard needs a wrapper that maps to its actual LAYOUT macro.
 * The ... and __VA_ARGS__ pattern passes all arguments through unchanged.
 *
 * Why wrappers instead of using LAYOUT directly?
 * Because LAYOUT_split_3x6_3 vs LAYOUT_ortho_4x12 vs LAYOUT (mitosis) are
 * all different - the wrapper abstracts this away.
 */

// Mitosis: 3x5 + 2x4 thumb clusters = 46 keys
// The mitosis uses a flat LAYOUT() macro
#define LAYOUT_mitosis_wrapper(...) LAYOUT(__VA_ARGS__)

// Corne (Halcyon): 3x6 + 3 thumb keys per side = 42 keys
#define LAYOUT_corne_wrapper(...) LAYOUT_split_3x6_3(__VA_ARGS__)

// Kyria: 3x6 + 5 thumb keys per side + encoders = 50 keys
#define LAYOUT_kyria_wrapper(...) LAYOUT_split_3x6_5(__VA_ARGS__)

// Planck: 4x12 with 2u center space = 47 keys
// Two-stage expansion needed so row macros expand before LAYOUT counts args
#define LAYOUT_planck_1x2uC_wrapper(...) LAYOUT_planck_1x2uC(__VA_ARGS__)
#define LAYOUT_planck_wrapper(...) LAYOUT_planck_1x2uC_wrapper(__VA_ARGS__)

// Zima:
#define LAYOUT_zima_wrapper(...) macropad_rename_me(__VA_ARGS__)

/* ==========================================================================
 * 5-COLUMN MACROS (for 3x5 splits like the core of your layout)
 * ==========================================================================
 * These are your BASE rows - the 3x5 grid that's common to all boards.
 * Each macro is ONE ROW (5 keys) for either left (L) or right (R) hand.
 *
 * Naming convention: ___LAYER_ROW_SIDE___
 *   - Layer: BASE, LOWER, RAISE, etc.
 *   - Row: 1 (top), 2 (home), 3 (bottom)
 *   - Side: L (left), R (right)
 *
 * The triple underscores are just a visual convention (from Drashna) to make
 * it obvious these are wrapper macros, not regular defines.
 */

/* --------------------------------------------------------------------------
 * BASE LAYER - QWERTY with Home Row Mods
 * --------------------------------------------------------------------------
 *  Q     W     E     R     T          Y     U     I     O     P
 * A/Gui S/Alt D/Ctl F/Sft  G          H    J/Sft K/Ctl L/Alt ;/Gui
 *  Z     X     C     V     B          N     M     ,     .     /
 */
#define ___BASE_L1___ KC_Q, KC_W, KC_E, KC_R, KC_T
#define ___BASE_L2___ HM_A, HM_S, HM_D, HM_F, KC_G
#define ___BASE_L3___ KC_Z, KC_X, KC_C, KC_V, KC_B

#define ___BASE_R1___ KC_Y, KC_U, KC_I, KC_O, KC_P
#define ___BASE_R2___ KC_H, HM_J, HM_K, HM_L, HM_SCLN
#define ___BASE_R3___ KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH

/* --------------------------------------------------------------------------
 * VIM LAYER - Plain QWERTY without home row mods (for Vimium, etc.)
 * Combos: GACS mods on QW WE ER RT (left), YU UI IO OP (right)
 * --------------------------------------------------------------------------
 *  Q     W     E     R     T          Y     U     I     O     P
 *  A     S     D     F     G          H     J     K     L     ;
 *  Z     X     C     V     B          N     M     ,     .     /
 */
#define ___VIM_L1___ KC_Q, KC_W, KC_E, KC_R, KC_T
#define ___VIM_L2___ KC_A, KC_S, KC_D, KC_F, KC_G
#define ___VIM_L3___ KC_Z, KC_X, KC_C, KC_V, KC_B

#define ___VIM_R1___ KC_Y, KC_U, KC_I, KC_O, KC_P
#define ___VIM_R2___ KC_H, KC_J, KC_K, KC_L, KC_SCLN
#define ___VIM_R3___ KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH

/* --------------------------------------------------------------------------
 * RAISE LAYER - Symbols (Odin/Programming focused)
 * --------------------------------------------------------------------------
 *  \     $     |     -     [          ]     +     #     ;     ::
 *  ~     &     *     =     (          )     ^     _     %     :
 *  `     !     '     "     {          }     @     <     >     ?
 */
#define ___RAISE_L1___ KC_BSLS, KC_DLR, KC_PIPE, KC_MINS, KC_LBRC
#define ___RAISE_L2___ KC_TILD, KC_AMPR, KC_ASTR, KC_EQL, KC_LPRN
#define ___RAISE_L3___ KC_GRV, KC_EXLM, KC_QUOT, KC_DQT, KC_LCBR

#define ___RAISE_R1___ KC_RBRC, KC_PLUS, KC_HASH, KC_SCLN, KC_DCLN
#define ___RAISE_R2___ KC_RPRN, KC_CIRC, KC_UNDS, KC_PERC, KC_COLN
#define ___RAISE_R3___ KC_RCBR, KC_AT, KC_LT, KC_GT, KC_QUES

/* --------------------------------------------------------------------------
 * LOWER LAYER - Numbers and Media
 * --------------------------------------------------------------------------
 *  1     2     3     4     5          6     7     8     9     0
 *  _     _    Vol+  Vol-  Mute        .     4     5     6     0
 *  _     _    Prev  Play  Next        +     1     2     3     -
 */
#define ___LOWER_L1___ KC_1, KC_2, KC_3, KC_4, KC_5
#define ___LOWER_L2___ _______, _______, KC_VOLU, KC_VOLD, KC_MUTE
#define ___LOWER_L3___ _______, _______, KC_MPRV, KC_MPLY, KC_MNXT

#define ___LOWER_R1___ KC_6, KC_7, KC_8, KC_9, KC_0
#define ___LOWER_R2___ KC_DOT, KC_4, KC_5, KC_6, KC_0
#define ___LOWER_R3___ KC_PLUS, KC_1, KC_2, KC_3, KC_MINS

/* --------------------------------------------------------------------------
 * FUNC LAYER - F-keys and Navigation
 * --------------------------------------------------------------------------
 *  F9   F10   F11   F12   PrtSc       PgUp  Home  End   Ins   Del
 *  F5   F6    F7    F8     _          Left  Down  Up    Right  _
 *  F1   F2    F3    F4     _          PgDn   _     _     _      _
 */
#define ___FUNC_L1___ KC_F9, KC_F10, KC_F11, KC_F12, KC_PSCR
#define ___FUNC_L2___ KC_F5, KC_F6, KC_F7, KC_F8, _______
#define ___FUNC_L3___ KC_F1, KC_F2, KC_F3, KC_F4, _______

#define ___FUNC_R1___ KC_PGUP, KC_HOME, KC_END, KC_INS, KC_DEL
#define ___FUNC_R2___ KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, _______
#define ___FUNC_R3___ KC_PGDN, _______, _______, _______, _______

/* --------------------------------------------------------------------------
 * ADJUST LAYER - System (Tri-layer: LOWER + RAISE)
 * --------------------------------------------------------------------------
 * Base  Game   _     _    Caps        _     _     _     _    NKRO
 *  _     _     _     _    NumLk       _     _     _     _     _
 *  _     _     _     _     _          _     _     _     _     _
 */
#define ___ADJUST_L1___ TG(_BASE), TG(_GAMING), _______, _______, KC_CAPS
#define ___ADJUST_L2___ _______, _______, _______, _______, KC_NUM
#define ___ADJUST_L3___ _______, _______, _______, _______, _______

#define ___ADJUST_R1___ _______, _______, _______, _______, NK_TOGG
#define ___ADJUST_R2___ _______, _______, _______, _______, _______
#define ___ADJUST_R3___ _______, _______, _______, _______, _______

/* --------------------------------------------------------------------------
 * GAMING LAYER - No home row mods, standard QWERTY
 * --------------------------------------------------------------------------
 *  Q     W     E     R     T          Y     U     I     O     P
 *  A     S     D     F     G          H     J     K     L     ;
 *  Z     X     C     V     B          N     M     ,     .     /
 */
#define ___GAMING_L1___ KC_Q, KC_W, KC_E, KC_R, KC_T
#define ___GAMING_L2___ KC_A, KC_S, KC_D, KC_F, KC_G
#define ___GAMING_L3___ KC_Z, KC_X, KC_C, KC_V, KC_B

#define ___GAMING_R1___ KC_Y, KC_U, KC_I, KC_O, KC_P
#define ___GAMING_R2___ KC_H, KC_J, KC_K, KC_L, KC_SCLN
#define ___GAMING_R3___ KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH

/* --------------------------------------------------------------------------
 * ROGUELIKE LAYER - For diagonal movement roguelikes
 * Numpad on right for 8-directional movement, shifted alphas on left
 * --------------------------------------------------------------------------
 *  Q     W     E     R     T          _    KP7   KP8   KP9    _
 *  A     S     D     F     G          .    KP4   KP5   KP6    _
 *  Z     X     C     V     B          _    KP1   KP2   KP3    _
 */
#define ___ROGUELIKE_L1___ KC_Q, KC_W, KC_E, KC_R, KC_T
#define ___ROGUELIKE_L2___ KC_A, KC_S, KC_D, KC_F, KC_G
#define ___ROGUELIKE_L3___ KC_Z, KC_X, KC_C, KC_V, KC_B

#define ___ROGUELIKE_R1___ _______, KC_P7, KC_P8, KC_P9, _______
#define ___ROGUELIKE_R2___ KC_PDOT, KC_P4, KC_P5, KC_P6, _______
#define ___ROGUELIKE_R3___ _______, KC_P1, KC_P2, KC_P3, _______

/* --------------------------------------------------------------------------
 * SYS LAYER - System settings (hold Z and / for 1 sec to access)
 * --------------------------------------------------------------------------
 *  _    Base  Game   _     _          _     _     _     _    NKRO
 * Boot Reset   _     _     _          _     _     _   Reset Boot
 * hold   _     _     _     _          _     _     _     _   hold
 */
#define ___SYS_L1___ _______, TG(_BASE), TG(_GAMING), _______, _______
#define ___SYS_L2___ QK_BOOT, EE_CLR, _______, _______, _______
#define ___SYS_L3___ _______, _______, _______, _______, _______

#define ___SYS_R1___ _______, _______, _______, _______, NK_TOGG
#define ___SYS_R2___ _______, _______, _______, EE_CLR, QK_BOOT
#define ___SYS_R3___ _______, _______, _______, _______, _______

/* --------------------------------------------------------------------------
 * MOUSE LAYER - Mouse movement and buttons
 * --------------------------------------------------------------------------
 * WhlL  WhlD  WhlU  WhlR   _          _    Btn1  Btn2  Btn3   _
 * Acl0  MsL   MsD   MsU   MsR        MsL   MsD   MsU   MsR   Acl0
 * Acl1  Acl2  Btn1  Btn2  Btn3        _    Btn1  Btn2  Acl2  Acl1
 */
#define ___MOUSE_L1___ MS_WHLL, MS_WHLD, MS_WHLU, MS_WHLR, _______
#define ___MOUSE_L2___ MS_ACL0, MS_LEFT, MS_DOWN, MS_UP, MS_RGHT
#define ___MOUSE_L3___ MS_ACL1, MS_ACL2, MS_BTN1, MS_BTN2, MS_BTN3

#define ___MOUSE_R1___ _______, MS_BTN1, MS_BTN2, MS_BTN3, _______
#define ___MOUSE_R2___ MS_LEFT, MS_DOWN, MS_UP, MS_RGHT, MS_ACL0
#define ___MOUSE_R3___ _______, MS_BTN1, MS_BTN2, MS_ACL2, MS_ACL1

/* --------------------------------------------------------------------------
 * BLANK TEMPLATE - For new layers
 * --------------------------------------------------------------------------
 *  _     _     _     _     _          _     _     _     _     _
 *  _     _     _     _     _          _     _     _     _     _
 *  _     _     _     _     _          _     _     _     _     _
 */
#define ___BLANK_L1___ _______, _______, _______, _______, _______
#define ___BLANK_L2___ _______, _______, _______, _______, _______
#define ___BLANK_L3___ _______, _______, _______, _______, _______

#define ___BLANK_R1___ _______, _______, _______, _______, _______
#define ___BLANK_R2___ _______, _______, _______, _______, _______
#define ___BLANK_R3___ _______, _______, _______, _______, _______

/* ==========================================================================
 * THUMB CLUSTER MACROS - Keyboard Specific
 * ==========================================================================
 * Thumb clusters vary significantly between keyboards, so we define these
 * per-keyboard. Each keyboard's keymap can either:
 *   1. Use these predefined thumb macros
 *   2. Define custom thumbs inline in the keymap
 *
 * The Mitosis has 2x4 thumb clusters (8 keys per side, 16 total).
 */

// clang-format off
/* --------------------------------------------------------------------------
 * MITOSIS THUMBS (2 rows of 4 keys per side)
 * --------------------------------------------------------------------------
 * Left:                          Right:
 * OSShft OSCtl OSAlt  ___        CapsWd OSL(FN) OSL(ADJ) ___
 * Esc    Ctrl  Bksp   Spc/Rai    Ent/Lo TAB/Fn  ___       ___
 */
#define ___MITOSIS_THUMB_BASE_L1___ OSM(MOD_LSFT), OSM(MOD_LCTL), OSM(MOD_LALT), _______
#define ___MITOSIS_THUMB_BASE_L2___ KC_ESC, KC_LCTL, KC_BSPC, SP_RAI

#define ___MITOSIS_THUMB_BASE_R1___ _______, _______, _______, _______
#define ___MITOSIS_THUMB_BASE_R2___ ENT_LOW, FUN_TAB, _______, _______

/* Raise layer thumbs */
#define ___MITOSIS_THUMB_RAISE_L1___ _______, _______, KC_HMDR, KC_GRV
#define ___MITOSIS_THUMB_RAISE_L2___ _______, _______, _______, _______

#define ___MITOSIS_THUMB_RAISE_R1___ KC_ARROP, KC_ASSIGN, KC_SLSH, _______
#define ___MITOSIS_THUMB_RAISE_R2___ SP_LOW, _______, _______, _______

/* Lower layer thumbs */
#define ___MITOSIS_THUMB_LOWER_L1___ _______, _______, _______, _______
#define ___MITOSIS_THUMB_LOWER_L2___ _______, _______, _______, _______

#define ___MITOSIS_THUMB_LOWER_R1___ _______, _______, _______, _______
#define ___MITOSIS_THUMB_LOWER_R2___ _______, _______, _______, _______

/* Function layer thumbs */
#define ___MITOSIS_THUMB_FUNC_L1___ _______, _______, _______, _______
#define ___MITOSIS_THUMB_FUNC_L2___ _______, _______, _______, _______

#define ___MITOSIS_THUMB_FUNC_R1___ _______, TO(_BASE), _______, _______
#define ___MITOSIS_THUMB_FUNC_R2___ _______, _______, _______, _______

/* Adjust layer thumbs */
#define ___MITOSIS_THUMB_ADJUST_L1___ _______, _______, _______, _______
#define ___MITOSIS_THUMB_ADJUST_L2___ EE_CLR, QK_BOOT, _______, _______

#define ___MITOSIS_THUMB_ADJUST_R1___ _______, _______, _______, _______
#define ___MITOSIS_THUMB_ADJUST_R2___ _______, _______, _______, _______

/* Gaming layer thumbs */
#define ___MITOSIS_THUMB_GAMING_L1___ KC_LSFT, KC_ESC, KC_TAB, KC_1
#define ___MITOSIS_THUMB_GAMING_L2___ KC_LCTL, KC_LALT, KC_BSPC, KC_SPC

#define ___MITOSIS_THUMB_GAMING_R1___ KC_2, KC_3, KC_4, KC_5
#define ___MITOSIS_THUMB_GAMING_R2___ KC_ENT, KC_DEL, MO(_FUNCTION), TG(_BASE)

/* Roguelike layer thumbs */
#define ___MITOSIS_THUMB_ROGUELIKE_L1___ KC_LSFT, KC_ESC, KC_TAB, KC_1
#define ___MITOSIS_THUMB_ROGUELIKE_L2___ KC_LCTL, KC_LALT, KC_BSPC, KC_SPC

#define ___MITOSIS_THUMB_ROGUELIKE_R1___ KC_2, KC_3, KC_4, KC_5
#define ___MITOSIS_THUMB_ROGUELIKE_R2___ KC_ENT, KC_P0, MO(_FUNCTION), TG(_BASE)

/* VIM layer thumbs - transparent to allow RAISE/LOWER access */
#define ___MITOSIS_THUMB_VIM_L1___ _______, _______, _______, _______
#define ___MITOSIS_THUMB_VIM_L2___ _______, _______, _______, _______

#define ___MITOSIS_THUMB_VIM_R1___ _______, _______, _______, _______
#define ___MITOSIS_THUMB_VIM_R2___ _______, _______, _______, _______

/* SYS layer thumbs */
#define ___MITOSIS_THUMB_SYS_L1___ _______, _______, _______, _______
#define ___MITOSIS_THUMB_SYS_L2___ _______, _______, _______, _______

#define ___MITOSIS_THUMB_SYS_R1___ _______, _______, _______, _______
#define ___MITOSIS_THUMB_SYS_R2___ _______, _______, _______, _______

/* Mouse layer thumbs */
#define ___MITOSIS_THUMB_MOUSE_L1___ _______, _______, _______, _______
#define ___MITOSIS_THUMB_MOUSE_L2___ _______, _______, MS_BTN1, MS_BTN2

#define ___MITOSIS_THUMB_MOUSE_R1___ _______, _______, _______, _______
#define ___MITOSIS_THUMB_MOUSE_R2___ MS_BTN1, MS_BTN2, _______, _______

/* ==========================================================================
 * CORNE THUMBS (3 keys per side)
 * ==========================================================================
 * The Corne has fewer thumb keys, so we need to prioritize.
 *
 * Left:  Esc    Bksp   Spc/Rai
 * Right: Ent/Lo  '     Tab
 */

#define ___CORNE_THUMB_BASE___                                                 \
    KC_ESC, KC_BSPC, SP_RAI, ENT_LOW, KC_QUOT, KC_TAB

#define ___CORNE_THUMB_RAISE___                                                \
    _______, KC_HMDR, _______, SP_LOW, KC_ASSIGN, KC_ARROP

#define ___CORNE_THUMB_LOWER___                                                \
    _______, _______, _______, _______, _______, _______

#define ___CORNE_THUMB_FUNC___                                                 \
    _______, _______, _______, _______, TO(_BASE), _______

#define ___CORNE_THUMB_ADJUST___                                               \
    EE_CLR, QK_BOOT, _______, _______, _______, _______

#define ___CORNE_THUMB_GAMING___                                               \
    KC_LCTL, KC_LALT, KC_SPC, KC_ENT, MO(_FUNCTION), TG(_BASE)

#define ___CORNE_THUMB_ROGUELIKE___                                            \
    KC_LCTL, KC_LALT, KC_SPC, KC_ENT, KC_P0, TG(_BASE)

/* VIM layer thumbs - transparent to allow RAISE/LOWER access */
#define ___CORNE_THUMB_VIM___                                                  \
    _______, _______, _______, _______, _______, _______

#define ___CORNE_THUMB_SYS___                                                  \
    _______, _______, _______, _______, _______, _______

#define ___CORNE_THUMB_MOUSE___                                                \
    _______, MS_BTN2, MS_BTN1, MS_BTN1, MS_BTN2, _______

/*
 * ==========================================================================
 * KYRIA THUMBS (5 keys per side, plus the extra two on row 3)
 * ==========================================================================
 * The Kyria has many thumb keys
 *
 *                    +-------------.  ,-------------+
 *                    |      |      |  |      |      |
 +------+------+------+------+------|  |------+------+------+------+------+
 |encode|      |      |      |      |  |      |      |      |      |encode|
 |      |      |      |      |      |  |      |      |      |      |      |
 `----------------------------------'  `----------------------------------'
 * Left:  ___   ___   ___   ___   ___
 * Right: ___   ___   ___   ___   ___
 */

// clang-format off
#define ___KYRIA_THUMB_BASE___                                                 \
_______, _______, KC_ESC, KC_BSPC, SP_RAI,                                     \
ENT_LOW, KC_QUOT, KC_TAB, _______, _______

#define ___KYRIA_THUMB_RAISE___                                                \
_______, _______, _______, KC_HMDR, _______,                                   \
SP_LOW, KC_ASSIGN, KC_ARROP, _______, _______

#define ___KYRIA_THUMB_LOWER___                                                \
_______, _______, _______, _______, _______,                                   \
_______, _______, _______, _______, _______

#define ___KYRIA_THUMB_FUNC___                                                 \
_______, _______, _______, _______, _______,                                   \
_______, TO(_BASE), _______, _______, _______

#define ___KYRIA_THUMB_ADJUST___                                               \
_______, _______, EE_CLR, QK_BOOT, _______,                                    \
_______, _______, _______, _______, _______

#define ___KYRIA_THUMB_GAMING___                                               \
_______, _______, KC_LCTL, KC_LALT, KC_SPC,                                    \
KC_ENT, MO(_FUNCTION), TG(_BASE), _______, _______

#define ___KYRIA_THUMB_ROGUELIKE___                                            \
_______, _______, KC_LCTL, KC_LALT, KC_SPC,                                    \
KC_ENT, KC_P0, TG(_BASE), _______, _______

/* VIM layer thumbs - transparent to allow RAISE/LOWER access */
#define ___KYRIA_THUMB_VIM___                                                  \
_______, _______, _______, _______, _______,                                   \
_______, _______, _______, _______, _______

#define ___KYRIA_THUMB_SYS___                                                  \
_______, _______, _______, _______, _______,                                   \
_______, _______, _______, _______, _______

#define ___KYRIA_THUMB_MOUSE___                                                \
_______, _______, _______, MS_BTN2, MS_BTN1,                                   \
MS_BTN1, MS_BTN2, _______, _______, _______
// clang-format on

/* ==========================================================================
 * PLANCK BOTTOM ROW MACROS (11 keys - 2u spacebar takes one slot)
 * ==========================================================================
 * The Planck EZ with LAYOUT_planck_1x2uC has 4 rows of 12 keys, but the
 * bottom row has a 2u spacebar in the center, giving us 11 keys:
 *
 * ┌─────┬─────┬─────┬─────┬─────┬───────────┬─────┬─────┬─────┬─────┬─────┐
 * │  1  │  2  │  3  │  4  │  5  │   SPACE   │  6  │  7  │  8  │  9  │ 10  │
 * └─────┴─────┴─────┴─────┴─────┴───────────┴─────┴─────┴─────┴─────┴─────┘
 *
 * Format: 5 left keys, spacebar, 5 right keys (11 total)
 *
 * Base layer bottom row:
 * Ctrl  GUI   Alt   CapsW Raise    [Space]    Lower Left  Down  Up   Right
 */
// clang-format off
#define ___PLANCK_BOTTOM_BASE___                                               \
    KC_LCTL, KC_LGUI, KC_LALT, CW_TOGG, MO(_RAISE),                            \
    KC_SPC,                                                                    \
    MO(_LOWER), KC_LEFT, KC_DOWN, KC_UP, KC_RGHT

/* Raise layer bottom row */
#define ___PLANCK_BOTTOM_RAISE___                                              \
    _______, _______, _______, _______, _______,                               \
    _______,                                                                   \
    MO(_LOWER), _______, _______, _______, _______

/* Lower layer bottom row */
#define ___PLANCK_BOTTOM_LOWER___                                              \
    _______, _______, _______, _______, MO(_RAISE),                            \
    _______,                                                                   \
    _______, _______, _______, _______, _______

/* Function layer bottom row */
#define ___PLANCK_BOTTOM_FUNC___                                               \
    _______, _______, _______, _______, _______,                               \
    _______,                                                                   \
    _______, _______, _______, _______, _______

/* Adjust layer bottom row */
#define ___PLANCK_BOTTOM_ADJUST___                                             \
    _______, _______, _______, _______, _______,                               \
    _______,                                                                   \
    _______, EE_CLR, QK_BOOT, _______, _______

/* Gaming layer bottom row */
#define ___PLANCK_BOTTOM_GAMING___                                             \
    KC_LCTL, KC_LALT, KC_LGUI, TG(_GAMING), MO(_RAISE),                        \
    KC_SPC,                                                                    \
    MO(_LOWER), KC_LEFT, KC_DOWN, KC_UP, KC_RGHT

/* Roguelike layer bottom row */
#define ___PLANCK_BOTTOM_ROGUELIKE___                                          \
    KC_LCTL, KC_LALT, KC_LGUI, TG(_ROGUELIKE), _______,                        \
    KC_SPC,                                                                    \
    KC_P0, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT

/* VIM layer bottom row - transparent to allow RAISE/LOWER access */
#define ___PLANCK_BOTTOM_VIM___                                                \
    _______, _______, _______, _______, _______,                               \
    _______,                                                                   \
    _______, _______, _______, _______, _______

/* SYS layer bottom row */
#define ___PLANCK_BOTTOM_SYS___                                                \
    _______, _______, _______, _______, _______,                               \
    _______,                                                                   \
    _______, _______, _______, _______, _______

/* Mouse layer bottom row */
#define ___PLANCK_BOTTOM_MOUSE___                                              \
    _______, _______, _______, MS_BTN2, MS_BTN1,                               \
    MS_BTN1,                                                                   \
    MS_BTN2, _______, _______, _______, _______

/* ==========================================================================
 * 6-COLUMN EXPANSION MACROS
 * ==========================================================================
 * For keyboards with 6 columns per side (like Corne), we add an outer column.
 * These take the 5-column macros and add a key on each edge.
 *
 * Format: OUTER_LEFT, <5 col macro>, OUTER_RIGHT
 */

// Base layer with outer columns
#define ___BASE_L1_6___ KC_TAB, ___BASE_L1___
#define ___BASE_L2_6___ KC_ESC, ___BASE_L2___
#define ___BASE_L3_6___ KC_LSFT, ___BASE_L3___

#define ___BASE_R1_6___ ___BASE_R1___, KC_BSPC
#define ___BASE_R2_6___ ___BASE_R2___, KC_QUOT
#define ___BASE_R3_6___ ___BASE_R3___, KC_RSFT

// Raise layer with outer columns
#define ___RAISE_L1_6___ KC_GRV, ___RAISE_L1___
#define ___RAISE_L2_6___ _______, ___RAISE_L2___
#define ___RAISE_L3_6___ _______, ___RAISE_L3___

#define ___RAISE_R1_6___ ___RAISE_R1___, KC_BSPC
#define ___RAISE_R2_6___ ___RAISE_R2___, _______
#define ___RAISE_R3_6___ ___RAISE_R3___, _______

// Lower layer with outer columns
#define ___LOWER_L1_6___ KC_GRV, ___LOWER_L1___
#define ___LOWER_L2_6___ _______, ___LOWER_L2___
#define ___LOWER_L3_6___ _______, ___LOWER_L3___

#define ___LOWER_R1_6___ ___LOWER_R1___, KC_BSPC
#define ___LOWER_R2_6___ ___LOWER_R2___, _______
#define ___LOWER_R3_6___ ___LOWER_R3___, _______

// Function layer with outer columns
#define ___FUNC_L1_6___ _______, ___FUNC_L1___
#define ___FUNC_L2_6___ _______, ___FUNC_L2___
#define ___FUNC_L3_6___ _______, ___FUNC_L3___

#define ___FUNC_R1_6___ ___FUNC_R1___, _______
#define ___FUNC_R2_6___ ___FUNC_R2___, _______
#define ___FUNC_R3_6___ ___FUNC_R3___, _______

// Adjust layer with outer columns
#define ___ADJUST_L1_6___ _______, ___ADJUST_L1___
#define ___ADJUST_L2_6___ _______, ___ADJUST_L2___
#define ___ADJUST_L3_6___ _______, ___ADJUST_L3___

#define ___ADJUST_R1_6___ ___ADJUST_R1___, _______
#define ___ADJUST_R2_6___ ___ADJUST_R2___, _______
#define ___ADJUST_R3_6___ ___ADJUST_R3___, _______

// Gaming layer with outer columns
#define ___GAMING_L1_6___ KC_TAB, ___GAMING_L1___
#define ___GAMING_L2_6___ KC_ESC, ___GAMING_L2___
#define ___GAMING_L3_6___ KC_LSFT, ___GAMING_L3___

#define ___GAMING_R1_6___ ___GAMING_R1___, KC_BSPC
#define ___GAMING_R2_6___ ___GAMING_R2___, KC_QUOT
#define ___GAMING_R3_6___ ___GAMING_R3___, KC_ENT

// Roguelike layer with outer columns
#define ___ROGUELIKE_L1_6___ KC_TAB, ___ROGUELIKE_L1___
#define ___ROGUELIKE_L2_6___ KC_ESC, ___ROGUELIKE_L2___
#define ___ROGUELIKE_L3_6___ KC_LSFT, ___ROGUELIKE_L3___

#define ___ROGUELIKE_R1_6___ ___ROGUELIKE_R1___, _______
#define ___ROGUELIKE_R2_6___ ___ROGUELIKE_R2___, _______
#define ___ROGUELIKE_R3_6___ ___ROGUELIKE_R3___, _______

// VIM layer with outer columns
#define ___VIM_L1_6___ KC_TAB, ___VIM_L1___
#define ___VIM_L2_6___ KC_ESC, ___VIM_L2___
#define ___VIM_L3_6___ KC_LSFT, ___VIM_L3___

#define ___VIM_R1_6___ ___VIM_R1___, KC_BSPC
#define ___VIM_R2_6___ ___VIM_R2___, KC_QUOT
#define ___VIM_R3_6___ ___VIM_R3___, KC_RSFT

// SYS layer with outer columns
#define ___SYS_L1_6___ _______, ___SYS_L1___
#define ___SYS_L2_6___ _______, ___SYS_L2___
#define ___SYS_L3_6___ _______, ___SYS_L3___

#define ___SYS_R1_6___ ___SYS_R1___, _______
#define ___SYS_R2_6___ ___SYS_R2___, _______
#define ___SYS_R3_6___ ___SYS_R3___, _______

// Mouse layer with outer columns
#define ___MOUSE_L1_6___ _______, ___MOUSE_L1___
#define ___MOUSE_L2_6___ _______, ___MOUSE_L2___
#define ___MOUSE_L3_6___ _______, ___MOUSE_L3___

#define ___MOUSE_R1_6___ ___MOUSE_R1___, _______
#define ___MOUSE_R2_6___ ___MOUSE_R2___, _______
#define ___MOUSE_R3_6___ ___MOUSE_R3___, _______

// Blank layer with outer columns
#define ___BLANK_L1_6___ _______, ___BLANK_L1___
#define ___BLANK_L2_6___ _______, ___BLANK_L2___
#define ___BLANK_L3_6___ _______, ___BLANK_L3___

#define ___BLANK_R1_6___ ___BLANK_R1___, _______
#define ___BLANK_R2_6___ ___BLANK_R2___, _______
#define ___BLANK_R3_6___ ___BLANK_R3___, _______

/* ==========================================================================
 * KYRIA ROW 3 EXPANSION (8 keys - adds inner keys beside thumb cluster)
 * ==========================================================================
 * Inner extra keys on Kyria row 3:
 *                    +-------------.  ,-------------+
 *                    |these |these |  |these |these |
 * +------+------+------+------+------|  |------+------+------+------+------+
 * |encodr|      |      |      |      |  |      |      |      |      |encodr|
 * `----------------------------------'  `----------------------------------'
 */

// Base
#define ___BASE_L3_8___ ___BASE_L3_6___, _______, _______
#define ___BASE_R3_8___ _______, _______, ___BASE_R3_6___

// Lower
#define ___LOWER_L3_8___ ___LOWER_L3_6___, _______, _______
#define ___LOWER_R3_8___ _______, _______, ___LOWER_R3_6___

// Raise
#define ___RAISE_L3_8___ ___RAISE_L3_6___, _______, _______
#define ___RAISE_R3_8___ _______, _______, ___RAISE_R3_6___

// Adjust
#define ___ADJUST_L3_8___ ___ADJUST_L3_6___, _______, _______
#define ___ADJUST_R3_8___ _______, _______, ___ADJUST_R3_6___

// Func
#define ___FUNC_L3_8___ ___FUNC_L3_6___, _______, _______
#define ___FUNC_R3_8___ _______, _______, ___FUNC_R3_6___

// Gaming
#define ___GAMING_L3_8___ ___GAMING_L3_6___, _______, _______
#define ___GAMING_R3_8___ _______, _______, ___GAMING_R3_6___

// Gaming2
#define ___GAMING2_L3_8___ ___GAMING2_L3_6___, _______, _______
#define ___GAMING2_R3_8___ _______, _______, ___GAMING2_R3_6___

// Roguelike
#define ___ROGUELIKE_L3_8___ ___ROGUELIKE_L3_6___, _______, _______
#define ___ROGUELIKE_R3_8___ _______, _______, ___ROGUELIKE_R3_6___

// VIM
#define ___VIM_L3_8___ ___VIM_L3_6___, _______, _______
#define ___VIM_R3_8___ _______, _______, ___VIM_R3_6___

// SYS
#define ___SYS_L3_8___ ___SYS_L3_6___, _______, _______
#define ___SYS_R3_8___ _______, _______, ___SYS_R3_6___

// Mouse
#define ___MOUSE_L3_8___ ___MOUSE_L3_6___, _______, _______
#define ___MOUSE_R3_8___ _______, _______, ___MOUSE_R3_6___
// clang-format on
