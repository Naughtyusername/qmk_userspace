# Copyright 2025 naughtyusername
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Mitosis-specific build rules
#
# Most features are enabled in users/naughtyusername/rules.mk
# Only add Mitosis-specific things here.

# Mouse keys for the Adjust layer
MOUSEKEY_ENABLE = yes

# The Mitosis doesn't have RGB LEDs on the keyboard itself,
# just the receiver LED which is controlled differently.
RGBLIGHT_ENABLE = no
RGB_MATRIX_ENABLE = no
