/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * config.h - Shared configuration
 *
 * This file is automatically included for any keymap named "naughtyusername".
 * Settings here apply to ALL keyboards unless overridden in a keymap's
 * config.h.
 *
 * Note: Keymap-specific config.h files are processed AFTER this one,
 * so they can override these settings if needed.
 */

#pragma once

/* ==========================================================================
 * CAPS WORD
 * ==========================================================================
 * Caps Word is like Caps Lock, but smarter:
 *   - Automatically turns off after typing a word
 *   - Only affects letter keys (numbers, punctuation don't get shifted)
 *   - Activated by pressing both shifts together
 */
#define BOTH_SHIFTS_TURNS_ON_CAPS_WORD
#define CAPS_WORD_IDLE_TIMEOUT 3000 // Turn off after 3 seconds of no typing

/* ==========================================================================
 * HOME ROW MODS CONFIGURATION
 * ==========================================================================
 * These settings make home row mods usable without constant misfires.
 *
 * CHORDAL_HOLD: Uses which hand you press keys on to decide tap vs hold
 * PERMISSIVE_HOLD: Allows hold to register if another key is tapped during it
 * FLOW_TAP_TERM: Time window for fast typing to register as taps
 */
#define CHORDAL_HOLD
#define PERMISSIVE_HOLD
#define SPECULATIVE_HOLD

// Flow Tap prevents misfires during fast typing
// Lower = more aggressive (may miss intended holds)
// Higher = less aggressive (may have more misfires)
#define FLOW_TAP_TERM 100

/* ==========================================================================
 * TAPPING TERMS
 * ==========================================================================
 * TAPPING_TERM: How long you must hold a dual-function key for it to register
 *               as "hold" instead of "tap". Too short = accidental holds,
 *               too long = sluggish layer access.
 *
 * TAPPING_TERM_PER_KEY: Enables get_tapping_term() function for per-key tuning
 */
#define TAPPING_TERM 175
#define TAPPING_TERM_PER_KEY

// Quick tap: If you tap a key twice rapidly, the second tap is always a tap
// This prevents accidental holds when double-tapping
#define QUICK_TAP_TERM 120

/* ==========================================================================
 * TRI-LAYER
 * ==========================================================================
 * When LOWER and RAISE are both active, automatically activate ADJUST.
 * This is handled by update_tri_layer_state() in naughtyusername.c
 */
#define TRI_LAYER_LOWER_LAYER 1  // _LOWER
#define TRI_LAYER_UPPER_LAYER 2  // _RAISE
#define TRI_LAYER_ADJUST_LAYER 4 // _ADJUST

/* ==========================================================================
 * COMBOS
 * ==========================================================================
 * COMBO_TERM: Time window to press both keys for a combo to trigger
 * Too short = hard to hit combos, too long = accidental triggers
 */
#define COMBO_TERM 50

/* ==========================================================================
 * ONE-SHOT KEYS
 * ==========================================================================
 * One-shot modifiers (OSM) stay active for the next keypress only.
 * ONESHOT_TAP_TOGGLE: Tapping twice locks the one-shot (like Caps Lock)
 */
// #define ONESHOT_TAP_TOGGLE 2 // i dont think we need this at the moment. can
// make ifdef tho TODO

/* ==========================================================================
 * MOUSE KEYS (if enabled)
 * ==========================================================================
 * Tune these if mouse keys feel too fast/slow
 */
#ifdef MOUSEKEY_ENABLE
#define MOUSEKEY_TIME_TO_MAX 60
#define MOUSEKEY_MAX_SPEED 7
#endif

#ifndef MOUSEKEY_INTERVAL
#define MOUSEKEY_INTERVAL 16
#endif

#ifndef MOUSEKEY_WHEEL_DELAY
#define MOUSEKEY_WHEEL_DELAY 0
#endif

/* ==========================================================================
 * NKRO (N-Key Rollover)
 * ==========================================================================
 * Allows unlimited simultaneous keypresses (default USB limit is 6).
 * Essential for gaming and fast typing.
 */

// TODO need to do something to make these work in bootloaders. ie. this is off,
// bootmagic lite enabled, keys to enable this. etc.
#ifdef NKRO_ENABLE
#define NKRO_DEFAULT_ON true
#endif
