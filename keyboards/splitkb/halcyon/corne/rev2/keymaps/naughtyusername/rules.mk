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
HALCYON_KEYBOARD = yes

# TFT Display (left half) - NOTE: This is NOT an OLED!
# The Halcyon uses a TFT LCD which requires Quantum Painter, not OLED_ENABLE
# For now, leave display support disabled unless you want to implement Quantum Painter
# QUANTUM_PAINTER_ENABLE = yes  # Requires significant additional configuration
# OLED Display (left half) cant use here, tft display not usual oled. below is the correct way for this board
OLED_ENABLE = no

# Enable specific modules (examples)
# HLC_ENCODER = yes       # If you have an encoder module
HLC_TFT_DISPLAY = yes   # If you have a TFT RGB display
HLC_CIRQUE_TRACKPAD = yes # If you have a Cirque trackpad


# Cirque Trackpad (right half)
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = cirque_pinnacle_spi

# RGB Matrix (per-key RGB)
# The Halcyon Corne has RGB, enable if you want it
RGB_MATRIX_ENABLE = yes

# ==========================================================================
# HALCYON MODULES (Optional - Splitkb's module system)
# ==========================================================================
# If you want to use Splitkb's halcyon_modules userspace for advanced
# OLED/Cirque features, uncomment this and follow their setup guide.
# This would require additional setup - see docs.splitkb.com
#
# USER_NAME := halcyon_modules
#
# Then compile with:
#   qmk compile -kb splitkb/halcyon/corne/rev2 -km naughtyusername \
#       -e HLC_TFT_DISPLAY=1 -e HLC_CIRQUE_TRACKPAD=1

# ==========================================================================
# FIRMWARE SIZE OPTIMIZATION
# ==========================================================================
# The RP2040 has plenty of flash (16MB on Halcyon), but if you hit limits:

# Disable features you don't use
# MOUSEKEY_ENABLE = no      # We use pointing device instead
# SPACE_CADET_ENABLE = no
# GRAVE_ESCAPE_ENABLE = no

# Link-time optimization (usually already enabled)
LTO_ENABLE = yes
