// Copyright 2024 splitkb.com (support@splitkb.com)
// Copyright 2025 naughtyusername
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "qp.h"
#include "qp_surface.h"

// ==========================================================================
// Cyberpunk-neon color palette (HSV, 0-255 scale)
// ==========================================================================
// QP uses 0-255 for hue (not 0-360), so:
//   Cyan    ≈ 128    Magenta ≈ 213    Blue ≈ 170    Purple ≈ 191

// Tux image recolor: light pixels → cyan, dark pixels → black (blends with display bg)
#define HSV_TUX_FG  128, 255, 255
#define HSV_TUX_BG  0, 0, 0

// WPM number color
#define HSV_WPM     213, 255, 255

// Lock indicators: bright when active, dim when off
#define HSV_LOCK_ON   128, 255, 255
#define HSV_LOCK_OFF  128, 180, 80

// Per-layer name colors — cycle through the cyberpunk palette
// Stored in a struct array in the .c file, indexed by layer number

// ==========================================================================
// Device handles (shared for user-level access if needed)
// ==========================================================================

extern painter_device_t lcd;
extern painter_device_t lcd_surface;

// ==========================================================================
// Functions called from halcyon.c module system
// ==========================================================================

void backlight_wakeup(void);
void backlight_suspend(void);
