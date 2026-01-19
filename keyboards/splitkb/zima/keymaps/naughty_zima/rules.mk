# Splitkb Zima - Build rules
#
# NOTE: This is a standalone keymap - disable shared features
# that require definitions we don't have

# Skip userspace entirely for Zima
ifeq ($(strip $(KEYBOARD)), splitkb/zima)
    # Do nothing - Zima is standalone
else
    # ... rest of your rules.mk content ...
endif

# Explicitly disable features that need keyrecords.c
TAP_DANCE_ENABLE = no
COMBO_ENABLE = no
KEY_OVERRIDE_ENABLE = no

# Hardware features
OLED_ENABLE = yes
AUDIO_ENABLE = yes
RGBLIGHT_ENABLE = yes

# Encoder support
ENCODER_MAP_ENABLE = yes
