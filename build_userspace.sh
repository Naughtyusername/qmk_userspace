#!/bin/bash
# build_all.sh - Build all keyboards in external userspace
#
# Run from your qmk_userspace directory
#
# To regenerate compile_commands.json for clangd/LSP (kills uint16_t errors):
#   qmk compile -kb <keyboard> -km <keymap> --compiledb
# Only needed when adding new keyboards or after QMK updates.
# The .clangd file in this repo points at ~/qmk/qmk_firmware/ for the database.

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${YELLOW}=======================================${NC}"
echo -e "${YELLOW}  QMK Userspace Build - All Keyboards  ${NC}"
echo -e "${YELLOW}=======================================${NC}"

# Check we're in the right directory
if [[ ! -f "qmk.json" ]]; then
    echo -e "${RED}Error: qmk.json not found. Run this from your qmk_userspace directory.${NC}"
    exit 1
fi

FAILED=0

# Function to build a keyboard
build_kb() {
    local kb="$1"
    local km="$2"
    shift 2
    local extra_args="$@"

    echo -e "\n${CYAN}Building: ${kb}:${km}${NC}"
    if [[ -n "$extra_args" ]]; then
        echo -e "${CYAN}  Extra args: ${extra_args}${NC}"
    fi

    if qmk compile -kb "$kb" -km "$km" $extra_args; then
        echo -e "${GREEN}  ✓ ${kb}:${km} OK${NC}"
    else
        echo -e "${RED}  ✗ ${kb}:${km} FAILED${NC}"
        FAILED=1
    fi
}

echo -e "\n${GREEN}Starting builds...${NC}"

# Standard keyboards (no special flags needed)
build_kb "mitosis" "naughtyusername"
build_kb "splitkb/kyria/rev1" "naughtyusername"
build_kb "splitkb/zima" "naughty_zima"
build_kb "zsa/planck_ez/glow" "naughtyusername"

# Halcyon Corne - needs separate builds for each half
# Left half: TFT Display
build_kb "splitkb/halcyon/corne/rev2" "naughtyusername" \
    -e HLC_TFT_DISPLAY=1 \
    -e TARGET=corne_left_display

# Right half: Cirque Trackpad
build_kb "splitkb/halcyon/corne/rev2" "naughtyusername" \
    -e HLC_CIRQUE_TRACKPAD=1 \
    -e TARGET=corne_right_trackpad

# Results
echo -e "\n${YELLOW}=======================================${NC}"
if [[ $FAILED -eq 0 ]]; then
    echo -e "${GREEN}  ✅ ALL BUILDS COMPLETE${NC}"

    # List firmware files
    echo -e "\n${GREEN}Firmware files:${NC}"
    ls -la *.hex *.bin *.uf2 2>/dev/null | sed 's/^/  /' || echo "  (files in QMK firmware directory)"

    # Desktop notification
    if command -v notify-send &>/dev/null; then
        notify-send "QMK Build Complete" "All keyboards built successfully." -i keyboard
    fi
else
    echo -e "${RED}  ❌ SOME BUILDS FAILED${NC}"

    if command -v notify-send &>/dev/null; then
        notify-send "QMK Build Failed" "Check terminal for errors." -i dialog-error
    fi

    exit 1
fi
echo -e "${YELLOW}=======================================${NC}"
