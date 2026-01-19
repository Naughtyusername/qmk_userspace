# Copyright 2025 naughtyusername
# SPDX-License-Identifier: GPL-2.0-or-later
#
# rules.mk - Userspace build configuration
#
# This file is automatically included when building any keymap named
# "naughtyusername". It adds our source files and enables shared features.

# =============================================================================
# users/naughtyusername/rules.mk
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

# =============================================================================
# =============================================================================
# manual method
# Only include halcyon modules if explicitly requested by the keymap
# ifeq ($(HALCYON_KEYBOARD), yes)
#     HALCYON_USERSPACE := $(USER_PATH)/../../modules/splitkb/users/halcyon_modules
#     include $(HALCYON_USERSPACE)/rules.mk
# endif

# Auto-detect if this is a halcyon keyboard
ifneq (,$(findstring halcyon,$(KEYBOARD)))
    HALCYON_USERSPACE := $(USER_PATH)/../../modules/splitkb/users/halcyon_modules
    include $(HALCYON_USERSPACE)/rules.mk
endif

# =============================================================================
# =============================================================================
# super over complicated way to do things if we ever need it
# =============================================================================
# Halcyon module support
# These are conditionally compiled based on environment variables
# passed during compilation

# seems way overcomplicated but it works.
# =============================================================================
############################################################################################
# HALCYON_MODULES_PATH := $(USER_PATH)/../../modules/splitkb/users/halcyon_modules/splitkb #
# 																						   #
# # Base halcyon support (if any module is enabled)										   #
# ifneq (,$(filter 1,$(HLC_CIRQUE_TRACKPAD) $(HLC_TFT_DISPLAY) $(HLC_ENCODER)))			   #
#     SRC += $(HALCYON_MODULES_PATH)/halcyon.c											   #
#     VPATH += $(HALCYON_MODULES_PATH)													   #
# endif																					   #
# 																						   #
# # Cirque trackpad module																   #
# ifeq ($(HLC_CIRQUE_TRACKPAD), 1)														   #
#     VPATH += $(HALCYON_MODULES_PATH)/hlc_cirque_trackpad								   #
#     include $(HALCYON_MODULES_PATH)/hlc_cirque_trackpad/rules.mk						   #
# endif																					   #
# 																						   #
# # TFT display module																	   #
# ifeq ($(HLC_TFT_DISPLAY), 1)															   #
#     SRC += $(HALCYON_MODULES_PATH)/hlc_tft_display/hlc_tft_display.c					   #
#     VPATH += $(HALCYON_MODULES_PATH)/hlc_tft_display									   #
#     include $(HALCYON_MODULES_PATH)/hlc_tft_display/rules.mk							   #
# endif																					   #
# 																						   #
# # Encoder module																		   #
# ifeq ($(HLC_ENCODER), 1)																   #
#     SRC += $(HALCYON_MODULES_PATH)/hlc_encoder/hlc_encoder.c							   #
#     VPATH += $(HALCYON_MODULES_PATH)/hlc_encoder										   #
#     include $(HALCYON_MODULES_PATH)/hlc_encoder/rules.mk								   #
# endif																					   #
############################################################################################
