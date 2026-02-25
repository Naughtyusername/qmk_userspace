# QMK Tap-Hold Timing - Visual Decision Trees

## Decision Flow: Is This a TAP or HOLD?

### Step 1: Is a Tap-Hold Key Currently Active?

```
Key Event Received
    ↓
Is tapping_key active? (was a tap-hold key recently pressed)
    │
    ├─ YES → Go to Step 2: Active Key Decision
    │
    └─ NO → Is this event a tap-hold key press?
           │
           ├─ YES → Check FLOW_TAP_TERM
           │        │
           │        ├─ Within flow term? → Settle immediately as TAP
           │        │
           │        └─ Not in flow term? → Start tapping, set tapping_key
           │
           └─ NO → Process normally, update flow_tap tracking
```

---

### Step 2: Active Key Decision (Key Currently Deciding)

```
Tap-hold key is active (tapping_key)
    ↓
Has TAPPING_TERM expired?
    │
    ├─ NO (still within tapping term)
    │  ↓
    │  Did another key get pressed?
    │  │
    │  ├─ NO → Key still held, wait more
    │  │
    │  └─ YES → Another Key Interrupt (go to Step 3)
    │
    └─ YES (TAPPING_TERM expired)
       ↓
       Key still held?
       │
       ├─ YES → Settle as HOLD
       │
       └─ NO → Settle as TAP
```

---

### Step 3: Another Key Pressed (Interrupt)

```
Another key pressed while tap-hold is undecided
    ↓
Is CHORDAL_HOLD enabled?
    │
    ├─ YES → Check handedness
    │  │
    │  ├─ Same hand? → Settle as TAP immediately
    │  │
    │  └─ Opposite hand? → Continue to Step 4
    │
    └─ NO → Continue to Step 4
           ↓
Step 4: Check Decision Mode
```

---

### Step 4: Determine How to React to Interrupt

```
Another key pressed during TAPPING_TERM
    ↓
Which decision mode is active?
    │
    ├─ Default Mode
    │  │ Only full TAPPING_TERM timeout triggers hold
    │  │
    │  └─ Action: Buffer the other key, keep waiting
    │
    ├─ PERMISSIVE_HOLD
    │  │
    │  ├─ Other key is nested (press+release)?
    │  │  ├─ YES → Settle as HOLD
    │  │  └─ NO → Buffer and keep waiting
    │
    └─ HOLD_ON_OTHER_KEY_PRESS
       │
       └─ Any key pressed?
          └─ YES → Settle as HOLD immediately
```

---

## Decision Tree: QUICK_TAP_TERM (Sequential Taps)

```
Tap-hold key released (was tapped)
    ↓
User presses it again
    │
    ├─ Within QUICK_TAP_TERM?
    │  │
    │  ├─ YES → Sequential tap (increment tap.count)
    │  │        Auto-repeat behavior active
    │  │
    │  └─ NO → New press (reset tap.count = 0)
    │          Normal tapping term applies
    │
    └─ interrupted flag set? (another key was pressed before release)
       │
       ├─ YES → Don't allow sequential tap
       │        Even if within QUICK_TAP_TERM
       │
       └─ NO → Sequential tap is allowed
```

---

## Decision Tree: FLOW_TAP_TERM (Speed-Based)

```
New tap-hold key pressed
    ↓
No current tapping_key?
    │
    └─ YES
       ↓
       Calculate idle_time = now - flow_tap_prev_time
       │
       ├─ idle_time >= 500ms?
       │  │ (flow_tap_expired OR idle time > 500ms max)
       │  └─ Flow tap disabled for this press
       │
       └─ idle_time < 500ms?
          ↓
          Are BOTH this key AND previous key "flow tap keys"?
          (alphas, space, punctuation by default)
          │
          ├─ Both YES?
          │  │
          │  └─ Calculate flow_tap_term = get_flow_tap_term()
          │     │
          │     ├─ idle_time < flow_tap_term? (e.g., 30ms < 150ms)
          │     │  │
          │     │  └─ YES → Settle immediately as TAP
          │     │          Don't wait for TAPPING_TERM
          │     │
          │     └─ idle_time >= flow_tap_term?
          │        │
          │        └─ Use normal TAPPING_TERM
          │
          └─ Either NO?
             │
             └─ Use normal TAPPING_TERM
```

---

## State Machine: Complete Timeline

```
Time    Event                   tapping_key State       waiting_buffer      Action
─────────────────────────────────────────────────────────────────────────────────
0ms     SFT_T(A) down          { A, time=0 }           []                  Start tapping

50ms    KC_B down              { A, time=0 }           [B down]            Buffer B
        (within TAPPING_TERM)  (undecided)             (not processed)

80ms    KC_B released          { A, time=0 }           [B down, B up]      Buffer B release
        (still < 200ms)        (undecided)             (still not processed)

150ms   SFT_T(A) released      { }                     [B down, B up]      Settle A as TAP
        (< 200ms, so TAP)      (cleared)               (still buffered)
                               ↓                        ↓
                               Send 'a'                Start draining

        Process buffer[0]:      { B, time=50 }         [B up]              B becomes active
                               (new tapping_key)      (still buffered)     SFT_T settled

200ms   (B TAPPING_TERM)       { B, time=50 }         [B up]              (still deciding)

220ms   (Hypothetical B hold)   { }                    []                  B would settle
        Would happen at 250ms  (cleared after up)     (drained)           as TAP or HOLD
```

---

## CHORDAL_HOLD Decision Tree

```
Another key pressed while tap-hold undecided
    ↓
Is CHORDAL_HOLD enabled?
    │
    ├─ NO → Skip to main decision mode (see Step 4)
    │
    └─ YES → Determine handedness
       ↓
       Get handedness of tap-hold key
       │
       ├─ Left hand → Check other key
       │
       ├─ Right hand → Check other key
       │
       └─ Exempt (*) → Always allow hold

       Get handedness of other key
       │
       ├─ Left hand
       │  │
       │  ├─ Tap-hold is LEFT? → Same hand, settle as TAP
       │  │
       │  └─ Tap-hold is RIGHT? → Opposite hands, continue
       │
       ├─ Right hand
       │  │
       │  ├─ Tap-hold is RIGHT? → Same hand, settle as TAP
       │  │
       │  └─ Tap-hold is LEFT? → Opposite hands, continue
       │
       └─ Exempt (*) → Opposite hand rule doesn't apply
          Continue to normal decision

       (After chordal check)
       ↓
       Continue to Step 4: Check decision mode
```

---

## Configuration Decision: Which Options to Enable?

```
What's your typing style?
    │
    ├─ Slow/Careful → Want: Tight control, minimal surprises
    │                Recommended:
    │                • TAPPING_TERM = 250ms (longer window)
    │                • Default mode (no PERMISSIVE_HOLD)
    │                • No FLOW_TAP_TERM
    │                • Maybe CHORDAL_HOLD
    │
    ├─ Fast/Rolls keys → Want: Prevent accidental mods
    │                   Recommended:
    │                   • TAPPING_TERM = 200ms
    │                   • PERMISSIVE_HOLD (allow nested taps)
    │                   • CHORDAL_HOLD (block same-hand holds)
    │                   • FLOW_TAP_TERM = 150ms
    │
    └─ Gaming/Low latency → Want: Responsive mods
                           Recommended:
                           • TAPPING_TERM = 100-150ms (short)
                           • HOLD_ON_OTHER_KEY_PRESS (fastest)
                           • No CHORDAL_HOLD
                           • No FLOW_TAP_TERM
```

---

## Key Ordering in process_tapping()

```
Is there an active tapping_key?
    │
    NO → Reset state (lines 272-296)
    │   └─ Is this new event a tap-hold key press?
    │      └─ Call flow_tap_key_if_within_term()
    │         └─ If YES: flow tap applies, settle as TAP
    │      └─ If NO: Start new tapping_key
    │
    YES → Active key processing (lines 303-651)
    │    ├─ Was the key released? (lines 594-650)
    │    │  └─ Check QUICK_TAP_TERM for sequential taps
    │    │
    │    └─ Is key still held? (lines 304-592)
    │       ├─ Within TAPPING_TERM? (line 305)
    │       │  ├─ YES (lines 311-502)
    │       │  │  ├─ Did interrupt occur?
    │       │  │  │  ├─ CHORDAL_HOLD check (lines 346-362)
    │       │  │  │  ├─ PERMISSIVE_HOLD check (lines 369-401)
    │       │  │  │  └─ HOLD_ON_OTHER_KEY_PRESS check (lines 470-498)
    │       │  │  │
    │       │  │  └─ No interrupt: Just wait more
    │       │  │
    │       │  └─ NO (lines 545-591)
    │       │     └─ TAPPING_TERM expired
    │       │        ├─ If not yet decided (tap.count==0): Settle as HOLD
    │       │        └─ If already decided (tap.count>0): Process as normal
```

---

## Cascading Prevention: Waiting Buffer Mechanism

```
Event arrives
    ↓
Can process_tapping() handle it?
    │
    ├─ YES (tapping_key is decided or reset)
    │  └─ Return true
    │     └─ Continue to next event
    │
    └─ NO (tapping_key is undecided and event affects tapping)
       └─ Return false
          └─ Enqueue in waiting_buffer
             │
             ├─ Is there space?
             │  │
             │  ├─ YES → waiting_buffer[head] = event
             │  │         head = (head + 1) % 8
             │  │
             │  └─ NO → BUFFER OVERFLOW!
             │          ├─ clear_keyboard()
             │          ├─ waiting_buffer_clear()
             │          └─ Reset state machine (panic recovery)
             │
             └─ After event loop, drain buffer
                ├─ for each queued event:
                │  ├─ Call process_tapping() again
                │  │  ├─ Can process? Continue
                │  │  └─ Can't process? Stop draining
                │  │
                │  └─ Once processed, remove from buffer (tail++)
```

---

## Common Timing Scenarios

### Scenario 1: Distinct Taps (AABB pattern)

```
Timeline (TAPPING_TERM=200ms):
0────────────┬──────────────────200
│ SFT_T(A) down/up @150ms
│            │
│            ├─ Released before TAPPING_TERM
│            └─ Settles as TAP ('a')
│
│            205 → KC_B down
│            │     (Not in tapping term, processed normally)
│            │
│            215 → KC_B up

Result: 'a' then 'b' (both as taps, nothing shifted)
```

### Scenario 2: Nested Tap (ABBA pattern with PERMISSIVE_HOLD)

```
Timeline (TAPPING_TERM=200ms, PERMISSIVE_HOLD enabled):
0────────┬──┬─────────────┬──────────200
│ SFT down   B down        B up
│           (50ms)         (60ms)
│                          ↓
│                    Nested press detected
│                    Settle SFT as HOLD
│                    (not waiting full 200ms)
│
├─ 150ms: SFT released

Result: 'B' (shifted, because SFT was held)
```

### Scenario 3: Rolling Tap (ABAB pattern, default mode)

```
Timeline (TAPPING_TERM=200ms, Default mode):
0───────┬───┬─────────────┬────────200
│ SFT down  B down        SFT up
│         (50ms)         (150ms)
│                        ↓
│                   Released before
│                   TAPPING_TERM
│                   Settles as TAP
│
├─ 160ms: B up

Result: 'a' then 'b' (no shift because SFT released first)
```

### Scenario 4: Fast Typing with FLOW_TAP_TERM

```
Timeline (TAPPING_TERM=200ms, FLOW_TAP_TERM=150ms):
0─────────────┬─────────────────200
│ 'a' down    50ms: 'a' up
│             ↓ flow_tap_prev_time = 50ms
│
│             80ms: LSFT_T(KC_S) down
│                  ↓
│                  idle_time = 80 - 50 = 30ms
│                  30ms < 150ms FLOW_TAP_TERM?
│                  YES → Settle as TAP immediately!
│                  (don't wait 200ms for TAPPING_TERM)

Result: 'a' 's' (both sent fast, no 200ms delay)
```

---

## Decision Complexity Cheat Sheet

```
Simple Setup:
  No config → Default mode
  • Tap if released before TAPPING_TERM
  • Hold if held past TAPPING_TERM
  • Other keys buffered and delayed

Home Row Mods Setup:
  TAPPING_TERM_PER_KEY +
  PERMISSIVE_HOLD +
  CHORDAL_HOLD +
  FLOW_TAP_TERM

  • Longer TAPPING_TERM for home row keys
  • Allow nested taps to trigger hold
  • Prevent same-hand accidental holds
  • Fast typing disable hold behavior
  → Reduces accidental mods + maintains responsiveness

Gaming Setup:
  TAPPING_TERM = 100ms +
  HOLD_ON_OTHER_KEY_PRESS

  • Short decision window
  • Any key press → hold immediately
  → Most responsive, but aggressive

ZMK-Like Setup:
  TAPPING_TERM = 200ms +
  PERMISSIVE_HOLD +
  CHORDAL_HOLD +
  FLOW_TAP_TERM = 150ms

  • Behaves similar to ZMK's require-prior-idle
  • Spatial awareness (same vs opposite hand)
  • Speed-based fallback (FLOW_TAP_TERM)
```

---
