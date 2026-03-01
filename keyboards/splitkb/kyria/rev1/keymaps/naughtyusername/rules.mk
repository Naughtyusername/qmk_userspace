# Copyright 2025 naughtyusername
# SPDX-License-Identifier: GPL-2.0-or-later
#
# kyria/rev1-specific build rules

USER_NAME := naughtyusername

OLED_ENABLE = yes          # Enable OLED display
ENCODER_ENABLE = no       # Enabled - we have enough space now!
RGB_MATRIX_ENABLE = no     # Disable keyboard RGB matrix, as it is enabled by default on rev3
# RGBLIGHT_ENABLE = yes    # Disabled — forever being fucked by bytes on ATmega32U4
RGBLIGHT_ENABLE = no

# Custom changes - above is the defaults - 2026, first time back into qmk in 6 years!
SPLIT_KEYBOARD = yes
# SPLIT_OLED_ENABLE = yes  # Not needed - OLED_ENABLE handles both halves
SPLIT_WPM_ENABLE = no      # Disabled to save space (~500 bytes)

EXTRAKEY_ENABLE = yes					 # Audio control and System control
TRI_LAYER_ENABLE = yes
CAPS_WORD_ENABLE = yes

TAP_DANCE_ENABLE = no      # Disabled to save space (~500 bytes)
COMBO_ENABLE = yes
KEY_OVERRIDE_ENABLE = no   # Disabled to save space (~300 bytes)
LEADER_ENABLE = no         # Disabled to save space (~400 bytes) - reclaim for eager debounce
# too much space on kyria :( - saves about 1200 bytes turning this off
MOUSEKEY_ENABLE = no

# Enable NKRO (N-key rollover)
NKRO_ENABLE = no

# Bootmagic for easy bootloader access (lite version saves ~200 bytes)
BOOTMAGIC_ENABLE = no  # Temporarily disabled to save ~400 bytes for RGB+OLED

# Space saving options - these can be changed if we need them
CONSOLE_ENABLE = no  # enable when we need debugging only
COMMAND_ENABLE = no  # magic debugging - we have this off for caps word
SPACE_CADET_ENABLE = no
GRAVE_ESC_ENABLE = no
# MAGIC_ENABLE = no
UNICODE_ENABLE = no
SWAP_HANDS_ENABLE = no
# Enable link time optimization (saves space)
LTO_ENABLE = yes

# Use minimal printf implementation (saves ~400 bytes)
AVR_USE_MINIMAL_PRINTF = yes
