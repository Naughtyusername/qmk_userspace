/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * combos.h - Shared combo definitions (ported from ZMK)
 *
 * This file defines all combos using position-based detection via
 * COMBO_ONLY_FROM_LAYER. Combos trigger based on physical key positions
 * checked against the BASE layer, regardless of what's mapped on the
 * current active layer.
 */

#pragma once

#include "naughtyusername.h"

/* ==========================================================================
 * COMBO CONFIGURATION (also add these to config.h)
 * ==========================================================================
 */

// Position-based combos - always check against BASE layer keycodes
// This is what makes layer-switching combos work from GAMING2/ROGUELIKE
// even though those layers have different keycodes in those positions
#define COMBO_ONLY_FROM_LAYER 0

// Default timing (overridden per-combo below)
#define COMBO_TERM 30

// Enable per-combo customization
#define COMBO_TERM_PER_COMBO      // Different timeouts per combo
#define COMBO_MUST_HOLD_PER_COMBO // OSM combos require holding
#define COMBO_MUST_TAP_PER_COMBO  // HRM combos require tapping
#define COMBO_SHOULD_TRIGGER      // Layer-based filtering

// Timing tiers (matching ZMK)
#define COMBO_FAST 18 // One-shot mods - very tight to prevent misfires
#define COMBO_MED 30  // Standard utility combos
#define COMBO_SLOW 50 // Three-finger layer switches - more forgiving

/* ==========================================================================
 * COMBO INDEX ENUM
 * ==========================================================================
 * Naming: COMBO_<keys>_<action>
 * Order: Utility, Auto-pair, One-shot mods, Layer switches
 */
enum combo_names {
    // ===== UTILITY COMBOS (home row) =====
    COMBO_HJ_CAPSWORD, // H + J = Caps Word
    COMBO_JK_ESC,      // J + K = Escape
    COMBO_LSCLN_ENT,   // L + ; = Enter
    COMBO_FG_UNDS,     // F + G = Underscore
    COMBO_SD_BSPC,     // S + D = Backspace
    COMBO_DF_TAB,      // D + F = Tab

    // ===== AUTO-PAIR COMBOS (bottom row) =====
    COMBO_NM_PARENS,        // N + M = () with cursor inside
    COMBO_MCOMM_BRACES,     // M + , = {} with cursor inside
    COMBO_COMMDOT_BRACKETS, // , + . = [] with cursor inside
    COMBO_KCOMM_DQUOTES,    // K + , = "" with cursor inside
    COMBO_JM_SQUOTES,       // J + M = '' with cursor inside

    // ===== ONE-SHOT MODIFIER COMBOS (top row, GACS order) =====
    // Left hand
    COMBO_QW_OSM_LGUI, // Q + W = One-shot GUI
    COMBO_WE_OSM_LALT, // W + E = One-shot Alt
    COMBO_ER_OSM_LCTL, // E + R = One-shot Ctrl
    COMBO_RT_OSM_LSFT, // R + T = One-shot Shift
    // Right hand
    COMBO_YU_OSM_RSFT, // Y + U = One-shot Shift
    COMBO_UI_OSM_RCTL, // U + I = One-shot Ctrl
    COMBO_IO_OSM_RALT, // I + O = One-shot Alt
    COMBO_OP_OSM_RGUI, // O + P = One-shot GUI

    // ===== LAYER SWITCHING COMBOS =====
    // Universal escape hatch
    COMBO_QWE_TO_BASE, // Q + W + E = Return to BASE (from anywhere)

    // From BASE
    COMBO_ASD_TO_GAMING,  // A + S + D = Go to GAMING
    COMBO_NM_COMM_TO_VIM, // N + M + , = Go to VIM

    // From GAMING
    COMBO_YUI_TO_GAMING2,   // Y + U + I = Go to GAMING2
    COMBO_HJK_TO_ROGUELIKE, // H + J + K = Go to ROGUELIKE

    // Return routes (same combos, opposite direction)
    COMBO_YUI_TO_GAMING,   // Y + U + I = Return to GAMING (from GAMING2)
    COMBO_HJK_TO_GAMING,   // H + J + K = Return to GAMING (from ROGUELIKE)
    COMBO_NM_COMM_TO_BASE, // N + M + , = Return to BASE (from VIM)

    // Total count - MUST be last
    COMBO_LENGTH
};

/* ==========================================================================
 * COMBO KEY ARRAYS
 * ==========================================================================
 * All arrays use BASE layer keycodes (including home row mods).
 * COMBO_ONLY_FROM_LAYER makes these work on any layer by checking
 * physical positions against BASE.
 *
 * Requirements:
 *   - const uint16_t PROGMEM
 *   - End with COMBO_END
 */

// ----- Utility Combos -----
const uint16_t PROGMEM combo_hj[] = {KC_H, HM_J, COMBO_END};
const uint16_t PROGMEM combo_jk[] = {HM_J, HM_K, COMBO_END};
const uint16_t PROGMEM combo_lscln[] = {HM_L, HM_SCLN, COMBO_END};
const uint16_t PROGMEM combo_fg[] = {HM_F, KC_G, COMBO_END};
const uint16_t PROGMEM combo_sd[] = {HM_S, HM_D, COMBO_END};
const uint16_t PROGMEM combo_df[] = {HM_D, HM_F, COMBO_END};

// ----- Auto-Pair Combos -----
const uint16_t PROGMEM combo_nm[] = {KC_N, KC_M, COMBO_END};
const uint16_t PROGMEM combo_mcomm[] = {KC_M, KC_COMM, COMBO_END};
const uint16_t PROGMEM combo_commdot[] = {KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM combo_kcomm[] = {HM_K, KC_COMM, COMBO_END};
const uint16_t PROGMEM combo_jm[] = {HM_J, KC_M, COMBO_END};

// ----- One-Shot Mod Combos -----
// Left hand (GACS: GUI, Alt, Ctrl, Shift from pinky to index)
const uint16_t PROGMEM combo_qw[] = {KC_Q, KC_W, COMBO_END};
const uint16_t PROGMEM combo_we[] = {KC_W, KC_E, COMBO_END};
const uint16_t PROGMEM combo_er[] = {KC_E, KC_R, COMBO_END};
const uint16_t PROGMEM combo_rt[] = {KC_R, KC_T, COMBO_END};
// Right hand (GACS: Shift, Ctrl, Alt, GUI from index to pinky)
const uint16_t PROGMEM combo_yu[] = {KC_Y, KC_U, COMBO_END};
const uint16_t PROGMEM combo_ui[] = {KC_U, KC_I, COMBO_END};
const uint16_t PROGMEM combo_io[] = {KC_I, KC_O, COMBO_END};
const uint16_t PROGMEM combo_op[] = {KC_O, KC_P, COMBO_END};

// ----- Layer Switching Combos -----
const uint16_t PROGMEM combo_qwe[] = {KC_Q, KC_W, KC_E, COMBO_END};
const uint16_t PROGMEM combo_asd[] = {HM_A, HM_S, HM_D, COMBO_END};
const uint16_t PROGMEM combo_yui[] = {KC_Y, KC_U, KC_I, COMBO_END};
const uint16_t PROGMEM combo_hjk[] = {KC_H, HM_J, HM_K, COMBO_END};
const uint16_t PROGMEM combo_nm_comm[] = {KC_N, KC_M, KC_COMM, COMBO_END};

/* ==========================================================================
 * COMBO DEFINITIONS ARRAY
 * ==========================================================================
 * This MUST be named `key_combos` - QMK looks for this specific name.
 *
 * Note: Some combos share the same keys but have different layer filters
 * (handled in combo_should_trigger). For example:
 *   - COMBO_YUI_TO_GAMING2: fires on GAMING layer
 *   - COMBO_YUI_TO_GAMING: fires on GAMING2 layer
 */
combo_t key_combos[COMBO_LENGTH] = {
    // ===== UTILITY COMBOS =====
    [COMBO_HJ_CAPSWORD] = COMBO(combo_hj, CW_TOGG),
    [COMBO_JK_ESC] = COMBO(combo_jk, KC_ESC),
    [COMBO_LSCLN_ENT] = COMBO(combo_lscln, KC_ENT),
    [COMBO_FG_UNDS] = COMBO(combo_fg, KC_UNDS),
    [COMBO_SD_BSPC] = COMBO(combo_sd, KC_BSPC),
    [COMBO_DF_TAB] = COMBO(combo_df, KC_TAB),

    // ===== AUTO-PAIR COMBOS (use COMBO_ACTION for macros) =====
    [COMBO_NM_PARENS] = COMBO_ACTION(combo_nm),
    [COMBO_MCOMM_BRACES] = COMBO_ACTION(combo_mcomm),
    [COMBO_COMMDOT_BRACKETS] = COMBO_ACTION(combo_commdot),
    [COMBO_KCOMM_DQUOTES] = COMBO_ACTION(combo_kcomm),
    [COMBO_JM_SQUOTES] = COMBO_ACTION(combo_jm),

    // ===== ONE-SHOT MODIFIER COMBOS =====
    // Left hand
    [COMBO_QW_OSM_LGUI] = COMBO(combo_qw, OSM(MOD_LGUI)),
    [COMBO_WE_OSM_LALT] = COMBO(combo_we, OSM(MOD_LALT)),
    [COMBO_ER_OSM_LCTL] = COMBO(combo_er, OSM(MOD_LCTL)),
    [COMBO_RT_OSM_LSFT] = COMBO(combo_rt, OSM(MOD_LSFT)),
    // Right hand
    [COMBO_YU_OSM_RSFT] = COMBO(combo_yu, OSM(MOD_RSFT)),
    [COMBO_UI_OSM_RCTL] = COMBO(combo_ui, OSM(MOD_RCTL)),
    [COMBO_IO_OSM_RALT] = COMBO(combo_io, OSM(MOD_RALT)),
    [COMBO_OP_OSM_RGUI] = COMBO(combo_op, OSM(MOD_RGUI)),

    // ===== LAYER SWITCHING COMBOS =====
    // Universal escape - QWE always returns to BASE
    [COMBO_QWE_TO_BASE] = COMBO(combo_qwe, TO(_BASE)),

    // From BASE - outbound routes
    [COMBO_ASD_TO_GAMING] = COMBO(combo_asd, TO(_GAMING)),
    [COMBO_NM_COMM_TO_VIM] = COMBO(combo_nm_comm, TO(_VIM)),

    // From GAMING - outbound routes
    [COMBO_YUI_TO_GAMING2] = COMBO(combo_yui, TO(_GAMING2)),
    [COMBO_HJK_TO_ROGUELIKE] = COMBO(combo_hjk, TO(_ROGUELIKE)),

    // Return routes - same keys, different source layers
    [COMBO_YUI_TO_GAMING] = COMBO(combo_yui, TO(_GAMING)),
    [COMBO_HJK_TO_GAMING] = COMBO(combo_hjk, TO(_GAMING)),
    [COMBO_NM_COMM_TO_BASE] = COMBO(combo_nm_comm, TO(_BASE)),
};

/* ==========================================================================
 * COMBO CALLBACKS
 * ==========================================================================
 */

/**
 * Process custom combo actions (COMBO_ACTION entries)
 * Used for auto-pairing brackets/quotes with cursor positioning
 */
void process_combo_event(uint16_t combo_index, bool pressed) {
    if (!pressed)
        return; // Only act on key press, not release

    switch (combo_index) {
    case COMBO_NM_PARENS:
        // Type () and move cursor inside
        SEND_STRING("()" SS_TAP(X_LEFT));
        break;
    case COMBO_MCOMM_BRACES:
        // Type {} and move cursor inside
        SEND_STRING("{}" SS_TAP(X_LEFT));
        break;
    case COMBO_COMMDOT_BRACKETS:
        // Type [] and move cursor inside
        SEND_STRING("[]" SS_TAP(X_LEFT));
        break;
    case COMBO_KCOMM_DQUOTES:
        // Type "" and move cursor inside
        SEND_STRING("\"\"" SS_TAP(X_LEFT));
        break;
    case COMBO_JM_SQUOTES:
        // Type '' and move cursor inside
        SEND_STRING("''" SS_TAP(X_LEFT));
        break;
    }
}

/**
 * Per-combo timing control
 * Matches ZMK timing tiers: FAST (18ms), MED (30ms), SLOW (50ms)
 */
uint16_t get_combo_term(uint16_t combo_index, combo_t *combo) {
    switch (combo_index) {
    // One-shot mods need very tight timing to prevent misfires
    // during normal typing (we, er, etc. are common bigrams)
    case COMBO_QW_OSM_LGUI:
    case COMBO_WE_OSM_LALT:
    case COMBO_ER_OSM_LCTL:
    case COMBO_RT_OSM_LSFT:
    case COMBO_YU_OSM_RSFT:
    case COMBO_UI_OSM_RCTL:
    case COMBO_IO_OSM_RALT:
    case COMBO_OP_OSM_RGUI:
        return COMBO_FAST; // 18ms

    // Three-finger layer switches need more time
    case COMBO_QWE_TO_BASE:
    case COMBO_ASD_TO_GAMING:
    case COMBO_YUI_TO_GAMING2:
    case COMBO_YUI_TO_GAMING:
    case COMBO_HJK_TO_ROGUELIKE:
    case COMBO_HJK_TO_GAMING:
    case COMBO_NM_COMM_TO_VIM:
    case COMBO_NM_COMM_TO_BASE:
        return COMBO_SLOW; // 50ms

    // Everything else uses medium timing
    default:
        return COMBO_MED; // 30ms
    }
}

/**
 * Must-hold requirement for one-shot mod combos
 * Prevents accidental OSM activation during fast typing
 *
 * Behavior:
 *   - Tap Q+W quickly → normal QW output (no combo)
 *   - Hold Q+W → one-shot GUI activates
 */
bool get_combo_must_hold(uint16_t combo_index, combo_t *combo) {
    switch (combo_index) {
    case COMBO_QW_OSM_LGUI:
    case COMBO_WE_OSM_LALT:
    case COMBO_ER_OSM_LCTL:
    case COMBO_RT_OSM_LSFT:
    case COMBO_YU_OSM_RSFT:
    case COMBO_UI_OSM_RCTL:
    case COMBO_IO_OSM_RALT:
    case COMBO_OP_OSM_RGUI:
        return true;
    default:
        return false;
    }
}

/**
 * Must-tap requirement for combos containing home row mods
 * Prevents combo activation when trying to hold for the mod
 *
 * Behavior:
 *   - Tap J+K quickly → Escape (combo fires)
 *   - Hold J+K → Shift+Ctrl activates (no combo, mods work)
 */
bool get_combo_must_tap(uint16_t combo_index, combo_t *combo) {
    // Check if any key in the combo is a mod-tap
    uint16_t key;
    uint8_t idx = 0;
    while ((key = pgm_read_word(&combo->keys[idx])) != COMBO_END) {
        switch (key) {
        case QK_MOD_TAP ... QK_MOD_TAP_MAX:
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
            return true;
        }
        idx += 1;
    }
    return false;
}

/**
 * Layer-based combo filtering
 * Controls which combos fire on which layers
 *
 * This is crucial for layer-switching combos that share the same keys
 * but need different behaviors depending on the source layer.
 */
bool combo_should_trigger(uint16_t combo_index, combo_t *combo,
                          uint16_t keycode, keyrecord_t *record) {
    switch (combo_index) {
    // ===== UNIVERSAL ESCAPE HATCH =====
    // QWE returns to BASE from anywhere EXCEPT base itself
    case COMBO_QWE_TO_BASE:
        return !layer_state_is(_BASE);

    // ===== FROM BASE =====
    case COMBO_ASD_TO_GAMING:
    case COMBO_NM_COMM_TO_VIM:
        return layer_state_is(_BASE);

    // ===== FROM GAMING =====
    case COMBO_YUI_TO_GAMING2:
    case COMBO_HJK_TO_ROGUELIKE:
        return layer_state_is(_GAMING);

    // ===== RETURN ROUTES =====
    // YUI returns to GAMING only from GAMING2
    case COMBO_YUI_TO_GAMING:
        return layer_state_is(_GAMING2);

    // HJK returns to GAMING only from ROGUELIKE
    case COMBO_HJK_TO_GAMING:
        return layer_state_is(_ROGUELIKE);

    // NM, returns to BASE only from VIM
    case COMBO_NM_COMM_TO_BASE:
        return layer_state_is(_VIM);

    // ===== UTILITY & AUTO-PAIR COMBOS =====
    // These work on BASE and VIM only
    case COMBO_HJ_CAPSWORD:
    case COMBO_JK_ESC:
    case COMBO_LSCLN_ENT:
    case COMBO_FG_UNDS:
    case COMBO_SD_BSPC:
    case COMBO_DF_TAB:
    case COMBO_NM_PARENS:
    case COMBO_MCOMM_BRACES:
    case COMBO_COMMDOT_BRACKETS:
    case COMBO_KCOMM_DQUOTES:
    case COMBO_JM_SQUOTES:
        return layer_state_is(_BASE) || layer_state_is(_VIM);

    // ===== ONE-SHOT MOD COMBOS =====
    // These work on BASE and VIM (VIM has no HRM, so OSM is useful there)
    case COMBO_QW_OSM_LGUI:
    case COMBO_WE_OSM_LALT:
    case COMBO_ER_OSM_LCTL:
    case COMBO_RT_OSM_LSFT:
    case COMBO_YU_OSM_RSFT:
    case COMBO_UI_OSM_RCTL:
    case COMBO_IO_OSM_RALT:
    case COMBO_OP_OSM_RGUI:
        return layer_state_is(_BASE) || layer_state_is(_VIM);

    // Default: allow combo
    default:
        return true;
    }
}

/* ==========================================================================
 * CONFIG.H ADDITIONS
 * ==========================================================================
 * Copy these to your config.h if not already present:
 *
 * // Combo configuration
 * #define COMBO_ONLY_FROM_LAYER _BASE
 * #define COMBO_TERM 30
 * #define COMBO_TERM_PER_COMBO
 * #define COMBO_MUST_HOLD_PER_COMBO
 * #define COMBO_MUST_TAP_PER_COMBO
 * #define COMBO_SHOULD_TRIGGER
 *
 * ==========================================================================
 * RULES.MK ADDITIONS
 * ==========================================================================
 * COMBO_ENABLE = yes
 *
 * ==========================================================================
 * KEYRECORDS.C USAGE
 * ==========================================================================
 * In your keyrecords.c, add:
 *
 * #ifdef COMBO_ENABLE
 * #include "combos.h"
 * #endif
 */
