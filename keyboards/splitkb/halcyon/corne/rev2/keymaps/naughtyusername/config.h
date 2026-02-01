/* Copyright 2025 naughtyusername
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Halcyon Corne config - Board-specific overrides
 *
 * Most settings come from userspace config.h. This file only contains
 * settings that are specific to the Corne or override userspace defaults.
 */

#pragma once

/* ==========================================================================
 * SPLIT KEYBOARD SETTINGS
 * ========================================================================== */

// USB connection is on left half, trackpad is on right
// If you plug USB into the right side, comment this out
// #define MASTER_LEFT
// #define MASTER_RIGHT
// For EE_HANDS (set via eeprom), don't define MASTER_LEFT/RIGHT
// The Halcyon uses EE_HANDS by default
#define EE_HANDS

/* ==========================================================================
 * POINTING DEVICE - Cirque Trackpad (Right Half)
 * ==========================================================================
 * The Halcyon Corne connects the Cirque via SPI through the VIK connector.
 * These settings are for the 35mm curved overlay trackpad.
 *
 * VIK Pin mapping for Halcyon series (from keyboard.json):
 *   SPI_DRIVER = SPID1
 *   SPI_SCK  = GP14
 *   SPI_MOSI = GP15
 *   SPI_MISO = GP12
 *   CS       = GP13
 */
// this is important, we need this to compile
#define CIRQUE_PINNACLE_SPI_CS_PIN GP13
#ifdef POINTING_DEVICE_ENABLE
// Trackpad is on the right half
#define SPLIT_POINTING_ENABLE
#define POINTING_DEVICE_RIGHT
#define CIRQUE_PINNACLE_SECONDARY_TAP_ENABLE

// Cirque trackpad settings
// The Halcyon uses SPI, not I2C
#define CIRQUE_PINNACLE_DIAMETER_MM 35

// Rotation - adjust if cursor moves wrong direction
// Try 90, 180, 270 if movement is off
// #define POINTING_DEVICE_ROTATION_90
// #define POINTING_DEVICE_ROTATION_180
// #define POINTING_DEVICE_ROTATION_270

// Invert axes if needed
#define POINTING_DEVICE_INVERT_X
#define POINTING_DEVICE_INVERT_Y

// Sensitivity - higher = faster cursor
// Default is 1024, adjust to taste
// #define CIRQUE_PINNACLE_POSITION_SCALE 1024

// Tap to click - enable gestures
#define CIRQUE_PINNACLE_TAP_ENABLE
#define POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE

// Circular scroll - drag finger around edge to scroll
#define POINTING_DEVICE_GESTURES_SCROLL_ENABLE

// Attenuation (sensitivity filter) - lower = more sensitive
// Default is 4X, use 2X for thicker overlays (like curved)
// Options: CIRQUE_PINNACLE_ATTENUATION_1X, _2X, _3X, _4X
#define CIRQUE_PINNACLE_ATTENUATION EXTREG__TRACK_ADCCONFIG__ADC_ATTENUATE_2X

// Auto mouse layer - automatically switch to mouse layer when trackpad active
// Uncomment if you want this behavior
// #define POINTING_DEVICE_AUTO_MOUSE_ENABLE
// #define AUTO_MOUSE_DEFAULT_LAYER _LAYERWITHMICE
// #define AUTO_MOUSE_TIME 650

// SPI pins for Cirque (if not already defined by keyboard)
// this is defined correctly above this ifdef block.
// #define SPI_DRIVER SPID0
// #define SPI_SCK_PIN GP22
// #define SPI_MOSI_PIN GP23
// #define SPI_MISO_PIN GP20
// #define CIRQUE_PINNACLE_SPI_CS_PIN // TODO possibly need to remove, make sure
// before flashing

#endif // POINTING_DEVICE_ENABLE

/* ==========================================================================
 * OLED DISPLAY (Left Half)
 * ========================================================================== */

#ifdef OLED_ENABLE
// OLED timeout (ms) - turn off after inactivity
#define OLED_TIMEOUT 30000

// Brightness (0-255)
#define OLED_BRIGHTNESS 128

// Update interval (ms)
#define OLED_UPDATE_INTERVAL 100
#endif

/* ==========================================================================
 * RGB MATRIX (Per-key RGB)
 * ==========================================================================
 * The Halcyon Corne has per-key RGB. Uncomment to customize.
 */

#ifdef RGB_MATRIX_ENABLE
// Startup settings
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_COLOR
#define RGB_MATRIX_DEFAULT_HUE 170 // Blue-ish
#define RGB_MATRIX_DEFAULT_SAT 255
#define RGB_MATRIX_DEFAULT_VAL 128 // 50% brightness

// Sleep behavior
#define RGB_MATRIX_SLEEP

// Disable some effects to save firmware space if needed
// #define DISABLE_RGB_MATRIX_ALPHAS_MODS
// #define DISABLE_RGB_MATRIX_GRADIENT_UP_DOWN
// ...etc
#endif

/* ==========================================================================
 * CORNE-SPECIFIC TIMING OVERRIDES
 * ==========================================================================
 * If the Corne needs different timing than your other boards, add here.
 * Otherwise, userspace config.h values are used.
 */

// Example: Corne might need slightly different tapping term
// #undef TAPPING_TERM
// #define TAPPING_TERM 180
