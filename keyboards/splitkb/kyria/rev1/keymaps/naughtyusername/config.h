/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Kyria-specific configuration overrides
 *
 * Most settings come from users/naughtyusername/config.h
 * Only put Kyria-specific things here.
 */

#pragma once

/* ==========================================================================
 * RGB UNDERGLOW - Minimal Config (Solid Colors Only)
 * ==========================================================================
 * Disable all animations to save ~1500-2000 bytes of firmware space.
 * Manual RGB control only (set hue/sat/val).
 */
#ifdef RGBLIGHT_ENABLE
// Disable all effects completely - manual control only
#define RGBLIGHT_DISABLE_KEYCODES  // Don't need RGB_ keycodes
#endif

/* ==========================================================================
 * FIRMWARE SIZE OPTIMIZATIONS
 * ========================================================================== */

// Use 16-bit layer state (we only have 10 layers, saves ~150 bytes)
#define LAYER_STATE_16BIT

// Disable unused features to save space
#undef LOCKING_SUPPORT_ENABLE      // Cherry MX lock switch support
#undef LOCKING_RESYNC_ENABLE
#define NO_MUSIC_MODE               // Disable music mode

/* OLED and encoder config can go here if needed */
