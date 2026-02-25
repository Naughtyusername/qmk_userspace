# QMK Tap-Hold Timing Research - Complete Index

## Overview

This is a comprehensive research package on QMK's tap-hold timing system. It answers all seven research questions from your original inquiry and provides the tools to understand and configure tap-hold behavior to match ZMK (or exceed it).

**Total Research**: ~70KB across 6 documents with 1100+ lines of QMK source analysis

---

## Documents

### 1. **RESEARCH_SUMMARY.md** (13 KB) - START HERE
**Your main reference document**

Quick overview of all findings with:
- Summary of each research question
- Key findings for each topic
- Comparison table (QMK vs ZMK)
- Why the timing feels different
- Code references
- Final insights

**Read first if**: You want the executive summary before diving deeper

---

### 2. **QMK_TAP_HOLD_TIMING_RESEARCH.md** (33 KB) - DEEP DIVE
**Comprehensive technical breakdown**

Detailed analysis of:
1. TAPPING_TERM, QUICK_TAP_TERM, FLOW_TAP_TERM interaction
2. Per-key tapping term behavior (override mechanics)
3. Three hold-tap decision modes (Default, Permissive, Hold-on-Other)
4. CHORDAL_HOLD mechanics and timing interaction
5. Cascading prevention (waiting buffer implementation)
6. require-prior-idle-ms equivalents
7. FLOW_TAP_TERM behavior (complete mechanics)
8. Key press rollover during tap-hold window

**Sections include**:
- Code snippets with file references
- Timeline examples
- Decision flow diagrams
- Macro explanations
- Complete state machine walkthrough

**Read if**: You want to understand the internals deeply

---

### 3. **QMK_TIMING_QUICK_REFERENCE.md** (11 KB) - CHEAT SHEET
**Practical configuration guide**

Quick reference with:
- Three timing constants explained
- Three decision modes at a glance
- CHORDAL_HOLD quick reference
- FLOW_TAP_TERM configuration templates
- Per-key configuration examples
- Common configuration patterns
- Troubleshooting guide
- Debugging tips

**Read if**: You just want to copy-paste a working config

---

### 4. **QMK_vs_ZMK_TIMING_COMPARISON.md** (16 KB) - ARCHITECTURE
**Why QMK feels different from ZMK**

Comparative analysis:
- Sequential (QMK) vs Parallel (ZMK) decision models
- Example: Three-key sequence flow in both systems
- Timing term interpretation differences
- Why FLOW_TAP_TERM was needed
- require-prior-idle-ms alternatives in QMK
- Cascading prevention mechanisms
- Configuration complexity comparison
- Performance implications
- Recommended QMK config for ZMK users

**Read if**: You're coming from ZMK and confused about behavior differences

---

### 5. **TIMING_DECISION_TREE.md** (8 KB) - VISUAL REFERENCE
**Visual flowcharts and decision trees**

Step-by-step decision flows:
- Step 1: Is a tap-hold key active?
- Step 2: Active key decision (during TAPPING_TERM)
- Step 3: Another key pressed (interrupt)
- Step 4: Decision mode reaction
- QUICK_TAP_TERM decision tree
- FLOW_TAP_TERM decision tree
- State machine complete timeline
- CHORDAL_HOLD decision tree
- Configuration decision guide
- Cascading prevention flowchart
- Common timing scenarios (4 detailed examples)

**Read if**: You're a visual learner or want to understand the decision flow

---

### 6. **QMK_TAP_HOLD_TIMING_RESEARCH.md** (COMPREHENSIVE)
**This is the main document you're reading from**

---

## Quick Navigation by Question

### Q1: How TAPPING_TERM, QUICK_TAP_TERM, and FLOW_TAP_TERM interact

**Files**:
- RESEARCH_SUMMARY.md → Section "1. Timing Constants"
- QMK_TAP_HOLD_TIMING_RESEARCH.md → Section "1. TAPPING_TERM, QUICK_TAP_TERM, FLOW_TAP_TERM Interaction"
- TIMING_DECISION_TREE.md → "FLOW_TAP_TERM Decision Tree"

**Key takeaway**: FLOW_TAP_TERM is a retrospective idle-time check, not a timeout. It can override TAPPING_TERM by forcing a TAP during fast typing.

---

### Q2: Per-key tapping term behavior

**Files**:
- RESEARCH_SUMMARY.md → Section "2. Per-Key Tapping Term"
- QMK_TAP_HOLD_TIMING_RESEARCH.md → Section "2. Per-Key Tapping Term Behavior"
- QMK_TIMING_QUICK_REFERENCE.md → "Per-Key Configuration"

**Key takeaway**: Per-key `get_tapping_term()` returns the exact timeout for that key. Other tap-holds still queue sequentially.

---

### Q3: Hold-tap flavors (decision modes)

**Files**:
- RESEARCH_SUMMARY.md → Section "3. Hold-Tap Flavors"
- QMK_TAP_HOLD_TIMING_RESEARCH.md → Section "3. Hold-Tap Flavors and Decision Modes"
- TIMING_DECISION_TREE.md → "Step 4: Determine How to React to Interrupt"
- QMK_TIMING_QUICK_REFERENCE.md → "Three Decision Modes"

**Key takeaway**: Three modes (Default, PERMISSIVE_HOLD, HOLD_ON_OTHER_KEY_PRESS) change WHEN the decision is made, not just what it is.

---

### Q4: CHORDAL_HOLD mechanics

**Files**:
- RESEARCH_SUMMARY.md → Section "4. CHORDAL_HOLD Mechanics"
- QMK_TAP_HOLD_TIMING_RESEARCH.md → Section "4. CHORDAL_HOLD Mechanics and Timing Interaction"
- TIMING_DECISION_TREE.md → "CHORDAL_HOLD Decision Tree"
- QMK_TIMING_QUICK_REFERENCE.md → "CHORDAL_HOLD" section

**Key takeaway**: CHORDAL_HOLD adds spatial logic (same vs opposite hand) to prevent same-hand accidental holds while allowing opposite-hand intentional ones.

---

### Q5: Cascading tap-hold issue prevention

**Files**:
- RESEARCH_SUMMARY.md → Section "5. Cascading Prevention"
- QMK_TAP_HOLD_TIMING_RESEARCH.md → Section "5. Cascading Tap-Hold Issue and Prevention"
- TIMING_DECISION_TREE.md → "Cascading Prevention Flowchart"
- QMK_vs_ZMK_TIMING_COMPARISON.md → "Cascading Prevention" section

**Key takeaway**: QMK uses a waiting_buffer (8-event capacity) to queue tap-holds sequentially. Only one key is actively deciding at a time.

---

### Q6: require-prior-idle-ms equivalent

**Files**:
- RESEARCH_SUMMARY.md → Section "6. Require-Prior-Idle Equivalent"
- QMK_TAP_HOLD_TIMING_RESEARCH.md → Section "6. QMK vs ZMK: require-prior-idle equivalent"
- QMK_vs_ZMK_TIMING_COMPARISON.md → "require-prior-idle-ms" section

**Key takeaway**: QMK has no direct equivalent. Use combination of PERMISSIVE_HOLD + CHORDAL_HOLD + FLOW_TAP_TERM for similar behavior.

---

### Q7: FLOW_TAP_TERM behavior

**Files**:
- RESEARCH_SUMMARY.md → Section "7. FLOW_TAP_TERM Behavior"
- QMK_TAP_HOLD_TIMING_RESEARCH.md → Section "7. FLOW_TAP_TERM Behavior Details"
- TIMING_DECISION_TREE.md → "FLOW_TAP_TERM Decision Tree"

**Key takeaway**: Checks if previous key was within FLOW_TAP_TERM milliseconds. If yes, immediately settle as TAP. Capped at 500ms idle time.

---

### Bonus: Key press rollover during tap-hold window

**Files**:
- QMK_TAP_HOLD_TIMING_RESEARCH.md → Section "8. Key Press Rollover"
- TIMING_DECISION_TREE.md → "Step 3: Another Key Pressed"

**Key takeaway**: Default mode buffers keys. HOLD_ON_OTHER_KEY_PRESS settles as hold. PERMISSIVE_HOLD checks if nested. FLOW_TAP_TERM forces tap.

---

## Configuration Recommendations by Use Case

### Home Row Mods (Recommended for most users)
```c
#define TAPPING_TERM 200
#define TAPPING_TERM_PER_KEY
#define PERMISSIVE_HOLD_PER_KEY
#define CHORDAL_HOLD
#define FLOW_TAP_TERM 150
```

See: QMK_TIMING_QUICK_REFERENCE.md → "Home Row Mods"

### Fast Typist (Minimal accidental mods)
```c
#define TAPPING_TERM 220
#define PERMISSIVE_HOLD
#define CHORDAL_HOLD
#define FLOW_TAP_TERM 120
```

See: QMK_TIMING_QUICK_REFERENCE.md → "Fast Typist"

### Gaming (Responsive mods)
```c
#define TAPPING_TERM 100
#define HOLD_ON_OTHER_KEY_PRESS
```

See: QMK_TIMING_QUICK_REFERENCE.md → "Gaming"

### ZMK Migration (Matching ZMK behavior)
```c
#define TAPPING_TERM 200
#define PERMISSIVE_HOLD_PER_KEY
#define CHORDAL_HOLD
#define FLOW_TAP_TERM 150
```

See: QMK_vs_ZMK_TIMING_COMPARISON.md → "Recommended QMK Config for ZMK Users"

---

## Code Reference Map

### Key Files in QMK
- `/home/naughty/qmk/qmk_firmware/quantum/action_tapping.h` — Macros and declarations
- `/home/naughty/qmk/qmk_firmware/quantum/action_tapping.c` — Full implementation (1100+ lines)

### Key Functions
- `action_tapping_process()` — Entry point
- `process_tapping()` — Main state machine (lines 253-651)
- `flow_tap_key_if_within_term()` — FLOW_TAP_TERM logic
- `waiting_buffer_enq()` — Queue management
- `get_chordal_hold()` — Spatial logic callback

### Key Macros
- `WITHIN_TAPPING_TERM(e)` — Check if event is within tapping window
- `WITHIN_QUICK_TAP_TERM(e)` — Check if event is within quick tap window
- `GET_TAPPING_TERM(keycode, record)` — Get timeout for specific key

---

## Quick Facts

1. **Only ONE tap-hold key is actively deciding at a time** (unlike ZMK's parallel model)
2. **FLOW_TAP_TERM is a retrospective check**, not a timeout
3. **Three decision modes exist**: Default, PERMISSIVE_HOLD, HOLD_ON_OTHER_KEY_PRESS
4. **CHORDAL_HOLD adds spatial logic** (same vs opposite hand)
5. **Waiting buffer prevents cascading** (8-event max capacity)
6. **No require-prior-idle equivalent** in QMK; use PERMISSIVE_HOLD + CHORDAL_HOLD instead
7. **FLOW_TAP_TERM is ZMK-like behavior** (disables hold during typing, enables after pause)
8. **Key press rollover is buffered** by default (delayed processing)

---

## Troubleshooting Quick Links

**Problem**: Keys are delayed during typing
- Solution: Enable FLOW_TAP_TERM
- Read: QMK_TIMING_QUICK_REFERENCE.md → "Troubleshooting"

**Problem**: Accidental mods in fast typing
- Solution: Enable PERMISSIVE_HOLD + CHORDAL_HOLD
- Read: QMK_TAP_HOLD_TIMING_RESEARCH.md → "Section 4"

**Problem**: Can't trigger modifier hold
- Solution: Reduce TAPPING_TERM or disable FLOW_TAP_TERM
- Read: QMK_TIMING_QUICK_REFERENCE.md → "Troubleshooting"

**Problem**: Doesn't feel like ZMK
- Solution: Enable PERMISSIVE_HOLD + CHORDAL_HOLD + FLOW_TAP_TERM
- Read: QMK_vs_ZMK_TIMING_COMPARISON.md → "Recommended Configuration"

---

## Document Statistics

| Document | Size | Lines | Focus |
|----------|------|-------|-------|
| RESEARCH_SUMMARY.md | 13 KB | 350 | Overview |
| QMK_TAP_HOLD_TIMING_RESEARCH.md | 33 KB | 800 | Deep dive |
| QMK_TIMING_QUICK_REFERENCE.md | 11 KB | 300 | Practical |
| QMK_vs_ZMK_TIMING_COMPARISON.md | 16 KB | 450 | Comparison |
| TIMING_DECISION_TREE.md | 8 KB | 250 | Visual |
| **TOTAL** | **~81 KB** | **~2,350** | Complete |

---

## How to Use These Documents

### First Time Learning
1. Read **RESEARCH_SUMMARY.md** (10 min)
2. Skim **TIMING_DECISION_TREE.md** (5 min)
3. Jump to relevant sections in **QMK_TAP_HOLD_TIMING_RESEARCH.md** (20 min)

### Configuring Your Keyboard
1. Check **QMK_TIMING_QUICK_REFERENCE.md** for your use case
2. Find configuration template
3. Adjust values based on your preferences
4. Test and iterate

### Coming from ZMK
1. Read **QMK_vs_ZMK_TIMING_COMPARISON.md** (15 min)
2. Find "Recommended Config" section
3. Copy configuration
4. Compare with QMK_TIMING_QUICK_REFERENCE.md

### Deep Understanding
1. Read all sections of **QMK_TAP_HOLD_TIMING_RESEARCH.md**
2. Cross-reference with **TIMING_DECISION_TREE.md**
3. Study **action_tapping.c** source code
4. Enable `ACTION_DEBUG` to see state machine logs

---

## Additional Resources

### In Your Userspace
All research documents are in `/home/naughty/qmk_userspace/`:
- QMK_TAP_HOLD_TIMING_RESEARCH.md
- QMK_TIMING_QUICK_REFERENCE.md
- QMK_vs_ZMK_TIMING_COMPARISON.md
- TIMING_DECISION_TREE.md
- RESEARCH_SUMMARY.md
- QMK_TIMING_RESEARCH_INDEX.md (this file)

### QMK Official Documentation
- `/home/naughty/qmk/qmk_firmware/docs/tap_hold.md`
- `/home/naughty/qmk/qmk_firmware/docs/mod_tap.md`

### QMK Source Code
- `/home/naughty/qmk/qmk_firmware/quantum/action_tapping.h` (configuration macros)
- `/home/naughty/qmk/qmk_firmware/quantum/action_tapping.c` (implementation)

---

## What's NOT Covered

- **Combos**: Separate system, briefly mentioned with CHORDAL_HOLD
- **Tap Dance**: Different mechanism entirely
- **One Shot Keys**: Uses tapping system but has unique behavior
- **Macro timing**: Not part of tap-hold system
- **Hardware debounce**: Affects tap-hold but not directly covered

---

## Questions This Research Answers

1. ✓ How TAPPING_TERM, QUICK_TAP_TERM, and FLOW_TAP_TERM interact
2. ✓ When get_tapping_term() returns a different value, does it override globally?
3. ✓ What are the different flavors and how do they affect timing?
4. ✓ How does CHORDAL_HOLD interact with TAPPING_TERM?
5. ✓ How does QMK prevent cascading tap-hold issue?
6. ✓ Is there a ZMK require-prior-idle equivalent?
7. ✓ Exactly when and how does FLOW_TAP_TERM override other timing?

**Bonus answer**: Key press rollover behavior during tap-hold window

---

## Final Notes

This research is based on:
- **QMK source code analysis** (action_tapping.c/h from December 2024 version)
- **Official QMK documentation** (docs/tap_hold.md and docs/mod_tap.md)
- **Detailed timeline walkthroughs** showing state machine behavior
- **Comparison with ZMK** to help users transitioning between systems

All code snippets, line numbers, and file paths are accurate as of the research date (Feb 24-25, 2026).

---
