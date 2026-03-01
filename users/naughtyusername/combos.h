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
 *                      COMBO REFERENCE & EDITING GUIDE
 * ==========================================================================
 *
 *  HOW THIS WORKS
 *  COMBO_ONLY_FROM_LAYER 0 means all combos match by physical position,
 *  checked against BASE layer keycodes. Use HM_ macros for home row mod
 *  positions (HM_A, HM_F, HM_J, etc.) in key arrays — plain KC_ won't match.
 *
 *  TO ADD A NEW COMBO
 *  1. Add enum entry to combo_names (before COMBO_LENGTH)
 *  2. Add PROGMEM key array (use HM_ for home row mod positions)
 *  3. Add entry to key_combos[] — COMBO() for keycodes, COMBO_ACTION() for
 macros
 *  4. If COMBO_ACTION: add case to process_combo_event()
 *  5. Add timing in get_combo_term() if not MED (default 30ms)
 *  6. Add to get_combo_must_hold() if it needs hold-to-activate (OSM combos)
 *  7. Add layer filter in combo_should_trigger()
 *
 *  TIMING TIERS
 *  FAST 18ms — one-shot mods (tight to avoid misfires on common bigrams)
 *  MED  30ms — utility combos (default, most combos use this)
 *  SLOW 50ms — three-finger layer switches (forgiving for 3 keys)
 *
 *  COMBO INDEX
 *  ┌────────────────────────┬──────┬────────────┬─────────┬──────────────────┐
 *  │ Enum                   │ Keys │ Output     │ Timing  │ Layers           │
 *  ├────────────────────────┼──────┼────────────┼─────────┼──────────────────┤
 *  │ UTILITY                │      │            │         │                  │
 *  │  COMBO_HJ_CAPSWORD     │ H+J  │ CW_TOGG    │ MED     │ BASE VIM         │
 *  │  COMBO_YH_NUMWORD      │ Y+H  │ NUMWORD    │ MED     │ BASE VIM         │
 *  │  COMBO_JK_ESC          │ J+K  │ KC_ESC     │ MED     │ BASE VIM         │
 *  │  COMBO_LSCLN_ENT       │ L+;  │ KC_ENT     │ MED     │ BASE VIM         │
 *  │  COMBO_FG_TAB          │ F+G  │ KC_TAB     │ MED     │ BASE VIM LOWER   │
 *  │  COMBO_DF_UNDS         │ D+F  │ KC_UNDS    │ MED     │ BASE VIM LOWER   │
 *  │  COMBO_AS_BSPC         │ A+S  │ KC_BSPC    │ MED     │ BASE VIM LOWER   │
 *  │  COMBO_HN_EQUAL        │ H+N  │ KC_EQUAL   │ MED     │ BASE VIM LOWER   │
 *  │                        │      │            │         │                  │
 *  │  COMBO_CV_MINS         │ C+V  │ KC_MINS    │ SLOW    │ BASE VIM LOWER   │
 *  │  COMBO_ZX_DEL          │ Z+X  │ KC_DEL     │ SLOW    │ BASE VIM LOWER   │
 *  │  COMBO_DC_ASSEQL       │ D+C  │ :=         │ SLOW    │ BASE VIM LOWER   │
 *  │  COMBO_GB_LEADER       │ G+B  │ QK_LEADER  │ SLOW    │ BASE VIM LOWER   │
 *  │  COMBO_VB_SPC          │ V+B  │ KC_SPC     │ SLOW    │ BASE VIM LOWER   │
 *  │  // XC - Reserved for future use                     │                  │
 *  │                        │      │            │         │                  │
 *  │  COMBO_KL_SQT          │ K+L  │ KC_QUOT    │ MED     │ BASE VIM         │
 *  │  COMBO_JKL_DQT         │ JKL  │ "          │ MED     │ BASE VIM         │
 *  │  COMBO_KL_SCLN_OCSC    │ KL;  │ : ^        │ MED     │ BASE VIM         │
 *  ├────────────────────────┼──────┼────────────┼─────────┼──────────────────┤
 *  │ AUTO-PAIR              │      │            │         │                  │
 *  │  COMBO_NM_PARENS       │ N+M  │ () + ←     │ MED     │ BASE VIM         │
 *  │  COMBO_MCOMM_BRACES    │ M+,  │ {} + ←     │ MED     │ BASE VIM         │
 *  │  COMBO_COMMDOT_BRACKETS│ ,+.  │ [] + ←     │ MED     │ BASE VIM         │
 *  │  COMBO_KCOMM_DQUOTES   │ K+,  │ "" + ←     │ MED     │ BASE VIM         │
 *  │  COMBO_JM_SQUOTES      │ J+M  │ '' + ←     │ MED     │ BASE VIM         │
 *  ├────────────────────────┼──────┼────────────┼─────────┼──────────────────┤
 *  │ ONE-SHOT MODS (GACS)   │      │            │         │                  │
 *  │  COMBO_RT_OSM_LSFT     │ R+T  │ OSM SHIFT  │ FAST    │ BASE VIM         │
 *  │  COMBO_YU_OSM_RSFT     │ Y+U  │ OSM SHIFT  │ FAST    │ BASE VIM         │
 *  │  COMBO_QW_OSM_LGUI     │ Q+W  │ OSM GUI    │ FAST    │ VIM (freed BASE) │
 *  │  COMBO_WE_OSM_LALT     │ W+E  │ OSM ALT    │ FAST    │ VIM (freed BASE) │
 *  │  COMBO_ER_OSM_LCTL     │ E+R  │ OSM CTRL   │ FAST    │ VIM (freed BASE) │
 *  │  COMBO_UI_OSM_RCTL     │ U+I  │ OSM CTRL   │ FAST    │ VIM (freed BASE) │
 *  │  COMBO_IO_OSM_RALT     │ I+O  │ OSM ALT    │ FAST    │ VIM (freed BASE) │
 *  │  COMBO_OP_OSM_RGUI     │ O+P  │ OSM GUI    │ FAST    │ VIM (freed BASE) │
 *  ├────────────────────────┼──────┼────────────┼─────────┼──────────────────┤
 *  │ LAYER SWITCHING        │      │            │         │                  │
 *  │  COMBO_QWE_TO_BASE     │ QWE  │ TO(BASE)   │ SLOW    │ !BASE (anywhere) │
 *  │  COMBO_ASD_TO_GAMING   │ ASD  │ TO(GAMING) │ SLOW    │ BASE             │
 *  │  COMBO_NM_COMM_TO_VIM  │ NM,  │ TO(VIM)    │ SLOW    │ BASE             │
 *  │  COMBO_YUI_TO_GAMING2  │ YUI  │ TO(GAMING2)│ SLOW    │ GAMING           │
 *  │  COMBO_HJK_TO_ROGUELIKE│ HJK  │ TO(ROGUE)  │ SLOW    │ GAMING           │
 *  │  COMBO_YUI_TO_GAMING   │ YUI  │ TO(GAMING) │ SLOW    │ GAMING2          │
 *  │  COMBO_HJK_TO_GAMING   │ HJK  │ TO(GAMING) │ SLOW    │ ROGUELIKE        │
 *  │  COMBO_NM_COMM_TO_BASE │ NM,  │ TO(BASE)   │ SLOW    │ VIM              │
 *  └────────────────────────┴──────┴────────────┴─────────┴──────────────────┘
    // For personal formatting use, templating etc.
 *  ┌────────────────────────┬──────┬────────────┬─────────┬──────────────────┐
 *  │                        │      │            │         │                  │
 *  ├────────────────────────┼──────┼────────────┼─────────┼──────────────────┤
 *  ├────────────────────────┼──────┼────────────┼─────────┼──────────────────┤
 *  └────────────────────────┴──────┴────────────┴─────────┴──────────────────┘
 *  │                        │      │            │         │                  │
 *
 *  OPEN SLOTS (no combo assigned):
 *    2-key:  XC, SD       (left hand)
 *    2-key:  QW, WE, ER, UI, IO, OP  (top row — AVAILABLE ON BASE ONLY, reserved for future use)
 *    3-key:  ZXC, ERT, DFG, CVB, IOP, ,./
 *    vert:   QA, WS, ED, RF, TG, UJ, IK, OL, P;, L., ;/
 *    cross:  A;, SL, DK, FJ
 *
 * ==========================================================================
 */
// clang-format off
enum combo_names {
    // ===== UTILITY COMBOS (home row) =====
    COMBO_HJ_CAPSWORD, // H + J = Caps Word
    COMBO_YH_NUMWORD,  // Y + H        = Num Word (numbers layer lock)

    COMBO_JK_ESC,      // J + K        = Escape
    COMBO_LSCLN_ENT,   // L + ;        = Enter
    COMBO_AS_BSPC,     // A + S        = Backspace
    COMBO_HN_EQUAL,    // H + N        = Equals

    COMBO_KL_SQT,      // K + L        = Single Quote
    COMBO_FG_TAB,      // F + G        = TAB
    COMBO_DF_UNDS,     // D + F        = Underscore
    COMBO_CV_MINS,     // C + V        = Minus
    COMBO_ZX_DEL,      // Z + X        = Delete
    COMBO_DC_ASSEQL,   // D + C        = := (assignment operator)
    COMBO_GB_LEADER,   // G + B        = Leader key start
    COMBO_VB_SPC,      // V + B        = Space

    // COMBO_SD_MINS,     // C + V        = Minus

    // COMBO_XC_XXX,      // X + C        =

    // Three Finger
    COMBO_JKL_DQT,     // J + K + L    = Double Quote
    COMBO_KL_SCLN_OCSC,// K + L + ;    = : ^ colon space caret combo - very
                       // useful in odin

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
const uint16_t PROGMEM combo_df[] = {HM_D, HM_F, COMBO_END};
const uint16_t PROGMEM combo_cv[] = {KC_C, KC_V, COMBO_END};
const uint16_t PROGMEM combo_zx[] = {LT(_SYS, KC_Z), KC_X, COMBO_END};
const uint16_t PROGMEM combo_dc[] = {HM_D, KC_C, COMBO_END};
const uint16_t PROGMEM combo_gb[] = {KC_G, KC_B, COMBO_END};
const uint16_t PROGMEM combo_vb[] = {KC_V, KC_B, COMBO_END};
const uint16_t PROGMEM combo_fg[] = {HM_F, KC_G, COMBO_END};
const uint16_t PROGMEM combo_as[] = {HM_A, HM_S, COMBO_END};
const uint16_t PROGMEM combo_hn[] = {KC_H, KC_N, COMBO_END};

const uint16_t PROGMEM combo_kl[] = {HM_K, HM_L, COMBO_END};
const uint16_t PROGMEM combo_yh[] = {KC_Y, KC_H, COMBO_END};

// ----- Three Finger Utility Combos
const uint16_t PROGMEM combo_jkl[] = {HM_J, HM_K, HM_L, COMBO_END};
const uint16_t PROGMEM combo_kl_scln[] = {HM_K, HM_L, HM_SCLN, COMBO_END};

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
    [COMBO_YH_NUMWORD] = COMBO(combo_yh, NUMWORD),
    [COMBO_JK_ESC] = COMBO(combo_jk, KC_ESC),
    [COMBO_LSCLN_ENT] = COMBO(combo_lscln, KC_ENT),
    [COMBO_DF_UNDS] = COMBO(combo_df, KC_UNDS),
    [COMBO_FG_TAB] = COMBO(combo_fg, KC_TAB),
    [COMBO_AS_BSPC] = COMBO(combo_as, KC_BSPC),
    [COMBO_HN_EQUAL] = COMBO(combo_hn, KC_EQL),
    [COMBO_CV_MINS] = COMBO(combo_cv, KC_MINS),
    [COMBO_ZX_DEL] = COMBO(combo_zx, KC_DEL),
    [COMBO_DC_ASSEQL] = COMBO(combo_dc, KC_ASSIGN),
    [COMBO_VB_SPC] = COMBO(combo_vb, KC_SPC),
#ifdef LEADER_ENABLE
    [COMBO_GB_LEADER] = COMBO(combo_gb, QK_LEADER),
#endif
    [COMBO_KL_SQT] = COMBO(combo_kl, KC_QUOT),

    [COMBO_JKL_DQT] = COMBO_ACTION(combo_jkl),
    [COMBO_KL_SCLN_OCSC] = COMBO_ACTION(combo_kl_scln),

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
    case COMBO_JKL_DQT:
        // Type "
        SEND_STRING("\"");
        break;
    case COMBO_KL_SCLN_OCSC:
        // Type ": ^" then one-shot shift (activates for next key, auto-releases)
        SEND_STRING(": ^");
        tap_code16(OSM(MOD_LSFT));
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
    // Bottom row combos get forgiving timing too
    case COMBO_QWE_TO_BASE:
    case COMBO_ASD_TO_GAMING:
    case COMBO_YUI_TO_GAMING2:
    case COMBO_YUI_TO_GAMING:
    case COMBO_HJK_TO_ROGUELIKE:
    case COMBO_HJK_TO_GAMING:
    case COMBO_NM_COMM_TO_VIM:
    case COMBO_NM_COMM_TO_BASE:
    case COMBO_ZX_DEL:
    case COMBO_CV_MINS:
    case COMBO_DC_ASSEQL:
    case COMBO_GB_LEADER:
    case COMBO_VB_SPC:
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
    // All top row OSM combos fire on tap, no hold required
    case COMBO_QW_OSM_LGUI:
    case COMBO_WE_OSM_LALT:
    case COMBO_ER_OSM_LCTL:
    case COMBO_RT_OSM_LSFT:
    case COMBO_YU_OSM_RSFT:
    case COMBO_UI_OSM_RCTL:
    case COMBO_IO_OSM_RALT:
    case COMBO_OP_OSM_RGUI:
        return false;
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

    // ===== UTILITY COMBOS (LEFT HAND + BOTTOM ROW) =====
    // These work on BASE, VIM, and LOWER (useful while holding LOWER with right hand)
    case COMBO_DF_UNDS:
    case COMBO_FG_TAB:
    case COMBO_AS_BSPC:
    case COMBO_HN_EQUAL:
    case COMBO_CV_MINS:
    case COMBO_ZX_DEL:
    case COMBO_DC_ASSEQL:
    case COMBO_GB_LEADER:
    case COMBO_VB_SPC:
        return layer_state_is(_BASE) || layer_state_is(_VIM) || layer_state_is(_LOWER);

    // ===== UTILITY & AUTO-PAIR COMBOS =====
    // These work on BASE and VIM only
    case COMBO_HJ_CAPSWORD:
    case COMBO_JK_ESC:
    case COMBO_LSCLN_ENT:
    case COMBO_KL_SQT:
    case COMBO_NM_PARENS:
    case COMBO_MCOMM_BRACES:
    case COMBO_COMMDOT_BRACKETS:
    case COMBO_KCOMM_DQUOTES:
    case COMBO_JM_SQUOTES:
    case COMBO_YH_NUMWORD:
    case COMBO_JKL_DQT:
    case COMBO_KL_SCLN_OCSC:
        return layer_state_is(_BASE) || layer_state_is(_VIM);

    // ===== ONE-SHOT MOD COMBOS =====
    // Shift combos work on BASE and VIM (useful for both regular typing and VIM)
    case COMBO_RT_OSM_LSFT:
    case COMBO_YU_OSM_RSFT:
        return layer_state_is(_BASE) || layer_state_is(_VIM);

    // GUI/ALT/CTRL combos scoped to VIM only (frees top row on BASE for future use)
    case COMBO_QW_OSM_LGUI:
    case COMBO_WE_OSM_LALT:
    case COMBO_ER_OSM_LCTL:
    case COMBO_UI_OSM_RCTL:
    case COMBO_IO_OSM_RALT:
    case COMBO_OP_OSM_RGUI:
        return layer_state_is(_VIM);

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
