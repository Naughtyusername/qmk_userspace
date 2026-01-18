# Copyright 2025 naughtyusername
# SPDX-License-Identifier: GPL-2.0-or-later
#
# rules.mk - Userspace build configuration
#
# This file is automatically included when building any keymap named
# "naughtyusername". It adds our source files and enables shared features.

# =============================================================================
# SOURCE FILES
# =============================================================================
# Tell QMK to compile our userspace C file
SRC += naughtyusername.c

# =============================================================================
# SHARED FEATURES
# =============================================================================
# These features are enabled for ALL keyboards using this userspace.
# Board-specific features should be enabled in the keymap's rules.mk instead.

# Core features we use everywhere
TAP_DANCE_ENABLE = yes      # For TD_Q_ESC
CAPS_WORD_ENABLE = yes      # Better than Caps Lock
COMBO_ENABLE = yes          # For J+K = Escape
KEY_OVERRIDE_ENABLE = yes   # For Shift+Bksp = Delete
TRI_LAYER_ENABLE = yes      # For LOWER + RAISE = ADJUST
EXTRAKEY_ENABLE = yes       # Media keys, volume control

# NKRO for unlimited simultaneous keys
NKRO_ENABLE = yes

# Bootmagic for easy bootloader access (hold top-left key on plug-in)
BOOTMAGIC_ENABLE = yes

# =============================================================================
# SPACE SAVING
# =============================================================================
# Disable features we don't use to save firmware space
# (Important for AVR boards with limited flash)

CONSOLE_ENABLE = no         # Only enable for debugging
COMMAND_ENABLE = no         # Magic key combos - conflicts with Caps Word

# Link-time optimization - can significantly reduce firmware size
LTO_ENABLE = yes
