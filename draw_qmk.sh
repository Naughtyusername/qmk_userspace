#!/bin/bash
# Usage: ./draw_qmk.sh <keyboard_path> <keymap_name>
# Example: ./draw_qmk.sh splitkb/kyria/rev1 naughtyusername

KB=$1
KM=$2
USER_DIR="users/naughtyusername"
MAP_FILE="keyboards/$KB/keymaps/$KM/keymap.c"

echo "🔍 Parsing QMK with Userspace Includes..."

# -I allows the preprocessor to find wrappers.h and combos.h
qmk c2json -kb "$KB" -km "$KM" \
    -I "$USER_DIR" \
    "$MAP_FILE" > temp_qmk.json

keymap parse -q temp_qmk.json > keymap.yaml
keymap draw --config config.yaml keymap.yaml > "${KM}_${KB////_}.svg"

rm temp_qmk.json
echo "✅ Done: ${KM}_${KB////_}.svg"
