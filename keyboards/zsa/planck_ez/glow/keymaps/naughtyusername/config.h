/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Planck EZ - Board-specific configuration overrides
 *
 * Most settings come from users/naughtyusername/config.h
 * Only Planck-specific things go here.
 */

#pragma once

/* ==========================================================================
 * AUDIO CONFIGURATION
 * ==========================================================================
 */
#ifdef AUDIO_ENABLE
#define AUDIO_CLICKY
#define AUDIO_CLICKY_FREQ_RANDOMNESS 0.5f
#endif

/* ==========================================================================
 * RGB MATRIX CONFIGURATION
 * ==========================================================================
 * The Planck EZ Glow has per-key RGB. Adjust brightness/effects as desired.
 */
#ifdef RGB_MATRIX_ENABLE
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_REACTIVE_SIMPLE
#define RGB_MATRIX_DEFAULT_HUE 170 // Blue-ish
#define RGB_MATRIX_DEFAULT_SAT 255
#define RGB_MATRIX_DEFAULT_VAL 128 // Half brightness
#endif
