# Copyright 2025 naughtyusername
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Halcyon Corne rules - Board-specific build options
#
# Common features are enabled in userspace rules.mk.
# This file enables Corne-specific hardware features.

# ==========================================================================
# HARDWARE FEATURES
# ==========================================================================

USER_NAME := naughtyusername

# Enable halcyon modules for this keyboard
# HALCYON_KEYBOARD = yes i dont think this does anything

# TFT Display (left half) - NOTE: This is NOT an OLED!
# The Halcyon uses a TFT LCD which requires Quantum Painter, not OLED_ENABLE
# For now, leave display support disabled unless you want to implement Quantum Painter
QUANTUM_PAINTER_ENABLE = yes  # Requires significant additional configuration
# OLED Display (left half) cant use here, tft display not usual oled. below is the correct way for this board
OLED_ENABLE = no

# Module selection — do NOT set these here!
# Each half needs a separate build with the correct module passed via -e flag:
#   Left:  qmk compile ... -e HLC_TFT_DISPLAY=1
#   Right: qmk compile ... -e HLC_CIRQUE_TRACKPAD=1
# Setting both here causes pin conflicts (both claim GP13 as SPI CS).
# HLC_ENCODER = 1
# HLC_TFT_DISPLAY = 1
# HLC_CIRQUE_TRACKPAD = 1

# Cirque Trackpad (right half)
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = cirque_pinnacle_spi

# RGB Matrix (per-key RGB)
# The Halcyon Corne has RGB, enable if you want it
RGB_MATRIX_ENABLE = yes

# WPM counter for TFT display
WPM_ENABLE = yes

# ==========================================================================
# FIRMWARE SIZE OPTIMIZATION
# ==========================================================================
# The RP2040 has plenty of flash (16MB on Halcyon), but if you hit limits:

# Disable features you don't use
MOUSEKEY_ENABLE = yes     # Needed for mouse buttons/accel on MOUSE layer (Cirque trackpad)
# GRAVE_ESCAPE_ENABLE = no

# Link-time optimization (usually already enabled)
LTO_ENABLE = yes
