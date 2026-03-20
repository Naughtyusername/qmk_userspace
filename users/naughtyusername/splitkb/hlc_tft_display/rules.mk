SRC += $(USER_PATH)/splitkb/hlc_tft_display/hlc_tft_display.c
POST_CONFIG_H += $(USER_PATH)/splitkb/hlc_tft_display/config.h

# Fonts
SRC += $(USER_PATH)/splitkb/hlc_tft_display/graphics/fonts/Retron2000-27.qff.c \
       $(USER_PATH)/splitkb/hlc_tft_display/graphics/fonts/Retron2000-underline-27.qff.c

# Tux image (mono4 palette — recolorable at draw time)
SRC += $(USER_PATH)/splitkb/hlc_tft_display/graphics/tux_100.qgf.c
