# QMK Leader Key Implementation Guide

Reference for adding leader key support to the userspace, triggered via combo.

## Overview

Leader key enters a mode where QMK buffers your next keypresses and matches them against defined sequences. When the sequence matches (or times out), QMK calls `leader_end_user()` where you define what each sequence does.

The pattern is the same as numword: combo triggers a mode, mode has its own processing, mode exits on timeout/completion.

## Files to Modify

### 1. `users/naughtyusername/rules.mk`

Add the feature flag:

```makefile
LEADER_ENABLE = yes
```

### 2. `users/naughtyusername/config.h`

Add timing config in a new section after the combo config block:

```c
/* ==========================================================================
 * LEADER KEY
 * ==========================================================================
 * LEADER_TIMEOUT: How long (ms) after pressing leader before it gives up
 * LEADER_PER_KEY_TIMING: Resets timeout after each keypress in the sequence
 *                        (without this, the whole sequence must complete
 *                        within LEADER_TIMEOUT of pressing leader)
 * LEADER_NO_TIMEOUT: Leader waits forever for input (not recommended)
 */
#define LEADER_TIMEOUT 300
#define LEADER_PER_KEY_TIMING
```

`LEADER_PER_KEY_TIMING` is important — without it, a 300ms timeout means your *entire* sequence must be typed within 300ms of the leader trigger. With it, you get 300ms *per key*, which is way more comfortable.

### 3. `users/naughtyusername/combos.h`

Uncomment and define a combo to trigger leader mode. Pick one of the open slots (ZX, XC, or VB — decide which feels right before starting).

Example using ZX:

In the enum, uncomment and rename:
```c
COMBO_ZX_LEADER,    // Z + X = Leader start
```

Add the key array (near the other utility combo arrays):
```c
const uint16_t PROGMEM combo_zx[] = {KC_Z, KC_X, COMBO_END};
```

Add to the `combo_actions[]` COMBO macro list:
```c
[COMBO_ZX_LEADER] = COMBO(combo_zx, QK_LEAD),
```

Add to `combo_should_trigger()` — probably allow on BASE and VIM only:
```c
case COMBO_ZX_LEADER:
    if (layer != _BASE && layer != _VIM) return false;
    break;
```

Add to `get_combo_term()` — COMBO_MED or COMBO_SLOW is fine since you're deliberately entering a mode:
```c
case COMBO_ZX_LEADER:
    return COMBO_MED; // 30ms — no need for speed here
```

### 4. `users/naughtyusername/naughtyusername.c`

Add the `leader_end_user()` callback. This is where all your sequences live. QMK calls this automatically when a leader sequence finishes.

```c
void leader_end_user(void) {
    // s -> e : send email
    if (leader_sequence_two_keys(KC_S, KC_E)) {
        SEND_STRING("youremail@example.com");
    }
    // s -> g : send github url
    if (leader_sequence_two_keys(KC_S, KC_G)) {
        SEND_STRING("github.com/naughtyusername/");
    }
    // Add more sequences here as needed
    // leader_sequence_one_key(KC_X)        — single key
    // leader_sequence_two_keys(KC_X, KC_Y) — two keys
    // leader_sequence_three_keys(...)      — three keys
    // leader_sequence_four_keys(...)       — four keys (max without modification)
    // leader_sequence_five_keys(...)       — five keys (max)
}
```

No changes needed to `process_record_user()` — QMK handles the leader key processing internally when it sees `QK_LEAD`.

## Sequence Ideas (from firmware.org)

| Sequence | Action | Notes |
|----------|--------|-------|
| `s` `e` | Send email address | SEND_STRING |
| `s` `g` | Send github URL | SEND_STRING |
| TBD | | |
| TBD | | |

## Timing Notes

- The combo trigger timing is separate from leader sequence timing
- Combo fires at whatever tier you assign (COMBO_MED recommended)
- Once in leader mode, `LEADER_TIMEOUT` (300ms) controls per-key timing
- If 300ms feels rushed, bump to 400ms — leader sequences are deliberate actions, not speed combos
- `LEADER_PER_KEY_TIMING` resets the clock after each key, so you only need to be fast *between* keys, not for the whole sequence

## Testing Checklist

- [ ] Firmware compiles with `LEADER_ENABLE = yes`
- [ ] Trigger combo enters leader mode (if your board has an LED/OLED you can add visual feedback later)
- [ ] Each defined sequence fires correctly
- [ ] Unrecognized sequences don't output garbage (QMK eats the buffered keys)
- [ ] Leader timeout feels comfortable — not too rushed, not too long
- [ ] Combo doesn't misfire during normal typing (Z+X shouldn't be a common bigram)
- [ ] Works on BASE and VIM layers, doesn't fire on GAMING layers
