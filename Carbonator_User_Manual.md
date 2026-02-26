# CARBONATOR — User Manual

### v2.1 | by MixedBySoda

---

## Table of Contents

1. [What Is Carbonator?](#what-is-carbonator)
2. [Interface Overview](#interface-overview)
3. [Controls](#controls)
4. [The Flavors — In Detail](#the-flavors--in-detail)
   - [Cola — Analog Console Warmth](#cola--analog-console-warmth)
   - [Cherry — Sweet Vocal Presence](#cherry--sweet-vocal-presence)
   - [Grape — Lo-Fi Tape Texture](#grape--lo-fi-tape-texture)
   - [Lemon-Lime — Crisp Exciter](#lemon-lime--crisp-exciter)
   - [Orange Cream — Resonant Lowpass Filter + Drive](#orange-cream--resonant-lowpass-filter--drive)
5. [The Carbonated Toggle](#the-carbonated-toggle)
6. [Signal Flow](#signal-flow)
7. [Tips & Tricks](#tips--tricks)
8. [System Requirements](#system-requirements)

---

## What Is Carbonator?

Carbonator is a multi-character audio effect plugin disguised as a soda machine. Each of its five **flavors** is a completely different DSP engine — from analog console saturation to tape wobble to multiband excitation — all controlled through a single intuitive knob called **Fizz**.

Think of it this way: you pick a flavor, twist the cap, and the audio gets more *carbonated*. Under the hood, the Fizz knob isn't a simple dry/wet mix. It's a **morph controller** that simultaneously reshapes multiple DSP parameters along carefully tuned curves — exponential drives, logarithmic filter sweeps, S-curve blends — so the effect always feels musical no matter where you set it.

Each flavor also has a **Carbonated/Flat** toggle that swaps between two distinct processing modes, effectively giving you **10 unique effects** in one plugin.

**Formats:** VST3, AU, AAX, Standalone (Universal Binary — Apple Silicon + Intel)

---

## Interface Overview

Carbonator's UI is styled like a vintage soda bottle — tall, narrow, and fizzy.

**Dimensions:** 420 × 560 pixels (resizable from 350×450 to 600×800)

### Layout (Top to Bottom)

```
┌──────────────────────────────┐
│  ☾               carbonator  │  ← Title bar (dark mode toggle top-left)
│     VINTAGE CARBONATED AUDIO │  ← Subtitle (color matches current flavor)
│                              │
│                              │
│            ╭─────╮           │
│           ╱ FIZZ  ╲          │  ← Animated title with flavor-specific effects
│          │  ⬤⬤⬤⬤  │         │  ← Bottle cap knob (ridged, rotary)
│          │  ⬤  ⬤  │         │     with pulsing rings around it
│          │  ⬤⬤⬤⬤  │         │
│           ╲______╱          │
│            67%               │  ← Fizz percentage readout
│                              │
│                              │
│  ┌─────────┬────────────┐   │
│  │ FLAVOR  │ CARBONATED │   │  ← Footer section
│  │  COLA   │  [rocker]  │   │     Flavor selector (left)
│  └─────────┴────────────┘   │     Carbonated toggle (right)
│                              │
│  ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒  │  ← Liquid fill animation (rises with Fizz)
│  ○  ○    ○   ○  ○   ○   ○  │  ← Bubble animation (when Carbonated is ON)
└──────────────────────────────┘
```

### Visual Details

- **Main Border:** 8px rounded rectangle in the current flavor's accent color. This is the first thing that tells you which flavor is active at a glance.
- **Liquid Fill:** A colored bar that rises from the bottom of the window proportional to the Fizz amount. At 0% the bottle is empty; at 100% it's full.
- **Bubbles:** 20 animated bubbles float upward when Carbonated mode is ON. Their speed scales with the Fizz amount (faster = more carbonation). They disappear entirely in Flat mode.
- **Glass Reflection:** A subtle white gradient on the top-right corner simulates light catching a glass bottle surface.
- **Noise Texture:** A very faint grain overlay across the entire UI for that analog, printed-label feel.

### Color Themes

Carbonator supports **Light** and **Dark** mode, toggled via the **☾** (moon) button in the top-left corner.

| Flavor | Light Mode Accent | Dark Mode Accent | Dark Mode Background |
|--------|-------------------|------------------|----------------------|
| **Cola** | Caramel `#c67c4e` | Warm Amber `#e8944a` | Dark Brown `#1e0f08` |
| **Cherry** | Red `#ff3b30` | Hot Pink `#ff0055` | Purple-Black `#1a0a2e` |
| **Grape** | Purple `#9b59b6` | Electric Violet `#bb00ff` | Deep Purple `#120a2e` |
| **Lemon-Lime** | Citrus Green `#7ec850` | Neon Green `#39ff14` | Green-Black `#0a1e08` |
| **Orange Cream** | Warm Orange `#ff8c42` | Bright Orange `#ff6b1a` | Amber-Black `#1e1208` |

**Light Mode Base:** Cream background (`#fff8f0`), dark text (`#2c2c2c`)
**Dark Mode Base:** Each flavor has its own dark background color (see table above), with lighter accent text

The entire UI — border, subtitle, knob glow, rocker switch LED, liquid fill, bubble glow — all shift to match the selected flavor's accent color.

---

## Controls

### Fizz Knob (The Bottle Cap)

The main event. A large rotary knob rendered as a **ridged bottle cap** with 21 scalloped edges, a concentric radial gradient for a 3D metallic look, and a white rectangular indicator notch showing the current position.

- **Range:** 0% – 100%
- **Interaction:** Click and drag vertically or horizontally to turn.
- **Display:** The current percentage is shown in large bold text (32pt) directly below the knob.

**What it does:** Fizz is **not** a wet/dry mix. It simultaneously morphs every parameter in the active flavor's DSP chain. At 0%, the effect is at its most subtle or bypassed state. At 100%, every parameter is at its most extreme. The morph curves are nonlinear — drives ramp exponentially (slow start, aggressive finish), filter cutoffs sweep logarithmically (big change early, fine-tuning later), and blend amounts follow S-curves (smooth transitions in the middle).

**Visual feedback while turning:**
- Three pulsing rings emanate from the knob (at 40px, 65px, and 90px radii). They pulse more intensely while you're actively dragging.
- A dashed spinning ring rotates slowly around the knob at low opacity.
- The Fizz title text above the knob has **per-flavor animations:**
  - **Cola:** Breathing heat glow with a character wave shimmer
  - **Cherry:** Sweeping sheen highlight with a bloom effect
  - **Grape:** Dithered shadow with specular highlights
  - **Lemon-Lime:** Crystalline shimmer with radiating light rays
  - **Orange Cream:** Warm radiating glow with a smoke/haze effect

### Flavor Selector

Located in the bottom-left footer panel.

- **Label:** "FLAVOR" (small, accent-colored)
- **Display:** The current flavor name in large bold text (28pt, accent-colored)
- **Interaction:** Click to open a dropdown menu with all five flavors.

Switching flavors completely swaps the DSP engine, color scheme, animations, and character of the plugin. Parameters (Fizz amount, Carbonated state, Output Gain) are preserved across flavor changes.

### Carbonated Toggle (The Rocker Switch)

Located in the bottom-right footer panel.

- **Label:** Reads **"CARBONATED"** when ON or **"FLAT"** when OFF. The label color changes to reflect the state.
- **Switch:** A vertical rocker switch (48×80px) styled with a dark body, gradient rocker element with highlight lines for a 3D hardware feel, and an **LED indicator** at the top that glows in the flavor's accent color when Carbonated is ON.

Each flavor has two completely different processing modes selected by this toggle. See the flavor descriptions below for exactly what changes.

### Output Gain

- **Range:** -12 dB to +12 dB
- **Default:** 0 dB

Final output level control applied after all processing. Use this to level-match with your dry signal.

### HQ Mode (Quality)

- **Default:** ON (enabled)

When ON, all saturation processing runs at **4× oversampling** using polyphase IIR half-band filters. This eliminates aliasing artifacts from the nonlinear waveshaping. Turn it OFF to save CPU if you're running many instances, at the cost of some high-frequency aliasing in the saturation stages.

### Bypass

Standard plugin bypass. When engaged, audio passes through unprocessed.

---

## The Flavors — In Detail

Each flavor is a complete, independent DSP engine. Below is exactly what happens to your audio in each one.

---

### Cola — Analog Console Warmth

**Character:** The sound of running your mix through a well-loved analog console. Soft harmonic saturation adds body, gentle compression glues things together, and a tilt EQ shifts the tonal balance from bright to warm.

**Color:** Caramel / Warm Amber

#### Carbonated Mode (ON)

**DSP Chain:**

1. **Asymmetric Soft Clip Saturation** (4× oversampled)
   The signal is pushed through an asymmetric soft clipper using the curve `x / (1 + |x|)` with a DC bias of 0.1. The asymmetry means positive and negative half-cycles are shaped differently, producing both **even and odd harmonics** — the same thing that gives analog circuits their "musical" distortion character. The DC bias shifts the waveform slightly off-center before clipping, creating that subtle second-harmonic warmth that engineers associate with transformer or tube coloration.

2. **DC Blocker** (5 Hz highpass)
   Removes the subsonic DC offset introduced by the asymmetric saturation so your speakers and meters stay happy.

3. **Soft-Knee Compressor**
   A program-dependent compressor with a 10ms attack and 100ms release. It doesn't slam; it *hugs*.

4. **Tilt EQ**
   A paired low shelf (200 Hz) and high shelf (8 kHz) that work together like a seesaw. As Fizz increases, the lows come up and the highs come down, tilting the tonal balance toward warmth.

**Fizz Knob Behavior (0% → 100%):**

| Parameter | 0% (Subtle) | 50% (Sweet Spot) | 100% (Pushed) | Curve |
|-----------|------------|-------------------|----------------|-------|
| Saturation Drive | 1.0× (unity) | ~2.0× | 4.0× | Exponential |
| Compressor Ratio | 1.5:1 | ~3:1 | 6:1 | Exponential |
| Compressor Threshold | -6 dB | -18 dB | -30 dB | Linear |
| Low Shelf Gain (200 Hz) | 0 dB | ~+2 dB | +3.5 dB | Logarithmic |
| High Shelf Gain (8 kHz) | 0 dB | ~-1.8 dB | -3.0 dB | Logarithmic |

**In plain English:** At low Fizz, Cola barely touches your signal — just a kiss of harmonic warmth. Around 40-60%, you get that classic "expensive console" sound — everything sits together, the low-mids fill out, and harsh highs are tamed. Past 75%, it starts to get noticeably compressed and colored — think "printed to tape through a Neve."

#### Flat Mode (OFF)

Replaces the full DSP chain with a simpler, more aggressive **inline tanh saturation** stage.

- Drive: 1.0× → 3.0×
- Normalized output: `tanh(x × drive) / drive` (prevents volume jumps)

This is a more direct, brighter saturation than Carbonated mode. No compression, no tilt EQ — just raw, controlled clipping. Good for adding edge and grit without the warmth shaping.

---

### Cherry — Sweet Vocal Presence

**Character:** Designed for vocals and melodic instruments. A parallel saturator adds harmonic richness while a surgical EQ chain carves out harshness and lifts presence and air. The result: your sound comes forward in the mix without getting brittle.

**Color:** Red / Hot Pink

#### Carbonated Mode (ON)

**DSP Chain:**

1. **Parallel Tanh Saturation** (4× oversampled)
   This is key — the saturation runs in **parallel**, not in series. The dry signal is blended with a tanh-saturated copy. Tanh produces predominantly **odd harmonics** (3rd, 5th, 7th...), giving a tube-like character. The blend amount is controlled by Fizz via an S-curve, ranging from 10% wet at Fizz 0% to 65% wet at Fizz 100%. This means even at maximum settings, you never lose your original signal's clarity — the harmonics are *added alongside*, not baked in destructively.

2. **De-Harsh Notch at 3.5 kHz**
   A narrow peak filter (Q=2.0) that dips out the harsh, nasal frequency range. At low Fizz it cuts up to -4 dB; as Fizz increases, the cut lessens (approaches 0 dB) because the presence boost above takes over the tonal shaping.

3. **Presence Bell at 4.5 kHz**
   A medium-width peak filter (Q=1.5) that boosts the "presence" zone — where consonants live and where vocals cut through a mix. Logarithmic scaling means the first 30% of Fizz travel gives you the most dramatic lift (+0 to ~+3.5 dB), then it refines from there to a maximum of +6 dB.

4. **Air Shelf at 12 kHz**
   A high shelf (Q=0.707) that adds breathy, open top-end. Ranges from 0 dB to +4 dB with Fizz.

**Fizz Knob Behavior (0% → 100%):**

| Parameter | 0% | 50% | 100% | Curve |
|-----------|-----|------|-------|-------|
| Parallel Blend | 10% wet | ~40% | 65% wet | S-Curve |
| Saturation Drive | 1.5× | ~2.8× | 4.5× | Exponential |
| Presence (+4.5 kHz) | 0 dB | ~+3.5 dB | +6 dB | Logarithmic |
| Air (+12 kHz) | 0 dB | ~+2.3 dB | +4 dB | Logarithmic |
| De-Harsh (3.5 kHz) | -4 dB | ~-1.5 dB | 0 dB | Logarithmic |

**In plain English:** Cherry makes things *shine*. Low Fizz values add a subtle sheen and forwardness — perfect on a lead vocal or acoustic guitar that needs to peek out. Mid-range Fizz is the vocal sweetener zone — rich harmonics, clear presence, open air, zero harshness. High Fizz pushes into "polished pop vocal" territory with heavy harmonic content and aggressive presence.

#### Flat Mode (OFF)

Swaps the EQ/saturation chain for a **subtle chorus effect**.

- Modulation Rate: 1.5 Hz (gentle, slow)
- Depth: 0.5ms → 1.0ms (S-curve with Fizz)
- Base Delay: 3ms
- Mix: 30% wet

A classic modulated delay chorus that adds width and movement. Think 80s vocal thickener or subtle string ensemble doubling. The depth increases with Fizz — from barely perceptible shimmer to noticeable pitch modulation.

---

### Grape — Lo-Fi Tape Texture

**Character:** The sound of a well-worn cassette deck. Tape saturation with warmth bias, wow & flutter pitch modulation, and a darkening lowpass filter that rolls off highs like magnetic tape losing its top end. Instant lo-fi nostalgia.

**Color:** Purple / Electric Violet

#### Carbonated Mode (ON)

**DSP Chain:**

1. **Tape Saturation** (4× oversampled, tanh with DC bias)
   Tanh waveshaping with a 0.15 DC bias — higher than Cola's 0.1 — giving a distinctly warmer, more even-harmonic character. This emulates the magnetic hysteresis of tape, where the oxide particles saturate asymmetrically at higher levels.

2. **DC Blocker** (5 Hz highpass)
   Cleans up the DC offset from the biased saturation.

3. **Wow & Flutter** (Modulated Delay)
   Two independent modulation sources applied to a delay line:
   - **Wow** (slow): A 0.4 Hz sine wave modulating the delay time by 0 to 3ms. This simulates the slow, heavy pitch drift caused by an uneven tape transport or a slightly warped capstan.
   - **Flutter** (fast): A 4.5 Hz asymmetric sawtooth (via arcsin shaping) modulating by 0 to 0.5ms. This simulates the rapid, jittery pitch instability from mechanical vibration in the tape path.
   - Base delay: 5ms. Buffer: 38,400 samples (handles up to 192 kHz sample rates).
   - Both use linear interpolation for smooth, artifact-free pitch shifts.

4. **Tape Head Lowpass** (State Variable TPF)
   A resonant lowpass filter that simulates high-frequency loss from tape head gap, oxide wear, and bias adjustment. The cutoff sweeps from 16 kHz down to 4 kHz as Fizz increases — like going from a freshly calibrated deck to a worn-out cassette that's been dubbed three times.

**Fizz Knob Behavior (0% → 100%):**

| Parameter | 0% | 50% | 100% | Curve |
|-----------|-----|------|-------|-------|
| Tape Drive | 1.2× | ~2.2× | 4.0× | Exponential |
| Wow Depth | 0 ms | ~1.0 ms | 3.0 ms | Exponential |
| Flutter Depth | 0 ms | ~0.17 ms | 0.5 ms | Exponential |
| LP Cutoff | 16 kHz | ~8 kHz | 4 kHz | Logarithmic |

**In plain English:** At low Fizz, Grape adds gentle tape warmth and barely-there wobble — your mix sounds like it was bounced through a well-maintained reel-to-reel. At mid Fizz, the wobble becomes audible and the top end starts to melt away — classic lo-fi bedroom pop territory. At full Fizz, it's a beat-up boombox eating your audio: heavy saturation, pronounced pitch warble, and a dark, muffled character that sounds like it's been passed through three generations of cassette dubs.

#### Flat Mode (OFF)

Swaps the tape emulation for a **vinyl record emulation**.

- **Crackle:** Sparse random pops at 0.1% → 1% trigger rate (scaled with Fizz). These are short, sharp transient bursts that simulate dust and groove wear on vinyl.
- **Rumble:** A 40 Hz low-frequency noise layer that simulates turntable motor rumble and groove vibration.
- **Mono Bass (stereo signals only):** Collapses stereo content below 300 Hz to mono, emulating the physical limitation of vinyl cutting lathes that can't handle out-of-phase bass (it makes the needle jump the groove).

A completely different lo-fi character — less "tape" and more "record player in a dusty attic."

---

### Lemon-Lime — Crisp Exciter

**Character:** A true multiband harmonic exciter. The signal is split at a variable crossover point; the low band passes through clean while the high band is saturated, compressed, and EQ'd. This adds brightness, presence, and "air" without touching the low end — the surgical approach to making things cut through a mix.

**Color:** Citrus Green / Neon Green

#### Carbonated Mode (ON)

**DSP Chain:**

1. **Linkwitz-Riley 4th-Order Crossover**
   The signal is split into two bands using cascaded State Variable TPF filters (two stages each for LR4 alignment, producing a -48 dB/octave slope). The crossover frequency sweeps from 4 kHz down to 1.5 kHz with Fizz — meaning at higher Fizz values, more of the mid-range gets pulled into the "excited" band.

   This is a proper Linkwitz-Riley crossover: when the bands are summed back together, they reconstruct to a perfectly flat magnitude response with zero phase offset at the crossover point. No comb filtering, no holes in the spectrum.

2. **HF Band Saturation** (4× oversampled, tanh — applied to high band only)
   The isolated high-frequency content is driven through tanh waveshaping. Since only harmonics of the high band are generated, there's no intermodulation with the bass content — you get clean, focused brightness enhancement. Drive ranges from 1.3× to 3.5× with Fizz.

3. **HF Envelope Compressor** (applied to high band only)
   A fast compressor (4:1 ratio, -20 dB threshold) that controls the dynamics of the excited high band so it doesn't spike or get harsh. The attack time shortens from 10ms to 0.5ms with Fizz — at high settings, it catches transients almost instantly for a smooth, controlled sizzle.

4. **Presence Bell at 5 kHz** (applied to high band)
   A peak filter (Q=1.5) boosting +0.5 dB to +6 dB. Adds focused presence energy.

5. **Air Shelf at 10 kHz** (applied to high band)
   A high shelf (Q=0.707) boosting +0.5 dB to +5 dB. Opens up the top end.

6. **Band Summation**
   Clean low band + processed high band are summed back together. The LR4 crossover ensures a mathematically perfect reconstruction with no artifacts at the crossover point.

**Fizz Knob Behavior (0% → 100%):**

| Parameter | 0% | 50% | 100% | Curve |
|-----------|-----|------|-------|-------|
| Crossover Frequency | 4 kHz | ~2.5 kHz | 1.5 kHz | Logarithmic |
| HF Drive | 1.3× | ~2.1× | 3.5× | Exponential |
| Presence (+5 kHz) | +0.5 dB | ~+3.5 dB | +6 dB | Logarithmic |
| Air (+10 kHz) | +0.5 dB | ~+3 dB | +5 dB | Logarithmic |
| Comp. Attack | 10 ms | ~3 ms | 0.5 ms | Exponential |

**In plain English:** Lemon-Lime is your scalpel. At low Fizz, it's a subtle high-end sweetener — everything above 4 kHz gets a whisper of harmonic richness and presence. At mid Fizz, the crossover drops to bring more of the upper-mids into play, the saturation starts to bite, and the presence/air boosts become clearly audible — acoustic guitars sparkle, hi-hats get crispy, vocals gain "expensive microphone" quality. At full Fizz, the crossover is down to 1.5 kHz, meaning everything from the upper-mids up is being saturated and boosted — aggressive exciter territory. The compressor keeps it controlled, but this is the "make it cut through a wall of sound" setting.

#### Flat Mode (OFF)

Swaps the multiband exciter for a **telephone/lo-fi bandpass EQ**.

- **Highpass Filter:** 300 Hz (removes all bass — like a phone speaker)
- **Lowpass Filter:** 3.5 kHz (removes all air and treble)
- **Resonance:** 0.707 → 3.0 (exponential with Fizz — higher values create a pronounced, nasal peak at the filter edges)

This creates the classic narrow-bandwidth "voice on a telephone" or "vintage radio broadcast" effect. At low resonance it's a clean bandpass; at high resonance the edges ring and it sounds like a lo-fi AM radio. A creative sound design tool for breakdowns, transitions, or intentional degradation.

---

### Orange Cream — Resonant Lowpass Filter + Drive

**Character:** A one-knob filter sweep with built-in saturation. Inspired by "OneKnob Filter" style plugins, but with a resonant edge. The Fizz knob sweeps a lowpass filter from wide open (20 kHz) down to nearly closed (200 Hz) while adding drive and bass compensation. It's the "DJ filter" flavor — simple, dramatic, and satisfying to automate.

**Color:** Warm Orange / Bright Orange

#### Carbonated Mode (ON)

**DSP Chain:**

1. **Warm Drive Saturation** (4× oversampled, WarmClip)
   Uses the gentlest saturation curve in the plugin: a cubic polynomial (`1.5x - 0.5x³`) that rounds peaks without adding aggressive harmonic content. This is pre-filter, so it colors the signal going *into* the lowpass — adding analog character to the filter sweep.

2. **Resonant Lowpass Filter** (4th-order, two-stage)
   - **Stage 1:** State Variable TPF with variable resonance (0.707 → 2.5 via S-curve). This is where the "filter" sound comes from — the resonant peak at the cutoff frequency that gives analog synth filters their characteristic vocal quality.
   - **Stage 2:** State Variable TPF with fixed Q=0.707 (Butterworth). Stacked on top of Stage 1 for a total **24 dB/octave** rolloff slope — steep enough to be dramatic, smooth enough to be musical.
   - **Cutoff Sweep:** 20 kHz → 200 Hz (logarithmic, curve=2.5). The logarithmic scaling is critical here — it means the knob spends more of its travel in the perceptually important range (1–8 kHz) and less time at the extremes.

3. **Low Shelf Boost at 200 Hz**
   As the lowpass closes and removes treble energy, the perceived loudness and body can drop off. This shelf compensates by adding 0 to +4 dB of low-end boost, keeping the bass full and present even as the filter sweeps down.

**Fizz Knob Behavior (0% → 100%):**

| Parameter | 0% | 50% | 100% | Curve |
|-----------|-----|------|-------|-------|
| Warm Drive | 1.0× | ~1.5× | 2.5× | Exponential |
| LP Cutoff | 20 kHz (open) | ~1.4 kHz | 200 Hz (nearly closed) | Logarithmic |
| Resonance (Q) | 0.707 (flat) | ~1.6 | 2.5 (singing) | S-Curve |
| Low Shelf (+200 Hz) | 0 dB | ~+2.5 dB | +4 dB | Logarithmic |

**In plain English:** At 0%, the filter is wide open — you hear your full signal with just a kiss of warm drive. As you push the Fizz up, the lowpass starts closing and you hear the familiar analog filter sweep — treble melting away, a resonant peak forming at the cutoff, the bass staying full. Around 50%, you're in classic "underwater" territory — think muffled DJ filter on the breakdown. At 100%, only deep bass and low mids survive through a tight, resonant 200 Hz lowpass with a warm, saturated edge. Automate this one for filter sweeps and transitions.

#### Flat Mode (OFF)

Same concept, but **dirtier and more extreme:**

| Parameter | Change from Carbonated |
|-----------|----------------------|
| Saturation | Switches from WarmClip to **tanh** — more aggressive, edgier harmonics |
| Drive Range | 1.5× → 4.0× (vs 1.0× → 2.5×) — nearly double the overdrive |
| LP Floor | Sweeps down to **100 Hz** (vs 200 Hz) — filter closes even further |
| Resonance | 0.707 → **4.0** (vs 2.5) — approaches self-oscillation territory |
| Stage 2 Q | 0.5× of Stage 1 resonance (adds edge vs fixed 0.707) |
| Low Shelf Boost | 0 → **+6 dB** (vs +4 dB) — heavier bass compensation |

Flat mode turns Orange Cream from a smooth DJ filter into a growling, aggressive synth filter. The resonance at 4.0 gets close to self-oscillation — the filter starts to "sing" at its cutoff frequency. Combined with the heavier tanh saturation, this mode is built for sound design, aggressive builds, and anything that needs to sound like it's being swallowed by a distorted subwoofer.

---

## The Carbonated Toggle

Every flavor has two modes — **Carbonated** (ON) and **Flat** (OFF) — selected by the rocker switch in the bottom-right corner.

The naming is thematic: "Carbonated" is the primary, more complex processing mode (full DSP chain with multiple stages). "Flat" is the alternate mode — sometimes simpler, sometimes weirder, always a distinctly different character.

### Summary Table

| Flavor | Carbonated (ON) | Flat (OFF) |
|--------|-----------------|------------|
| **Cola** | Asymmetric saturation + compression + tilt EQ | Raw inline tanh saturation |
| **Cherry** | Parallel saturation + presence/air EQ | Modulated chorus (1.5 Hz, 30% wet) |
| **Grape** | Tape saturation + wow/flutter + darkening LPF | Vinyl crackle + rumble + mono bass |
| **Lemon-Lime** | LR4 crossover multiband exciter | Telephone bandpass EQ (300–3500 Hz) |
| **Orange Cream** | Warm drive + resonant LP sweep (smooth) | Tanh drive + resonant LP sweep (aggressive) |

**Visual cue:** When Carbonated is ON, the rocker switch LED glows in the flavor's accent color, and animated bubbles float upward in the UI. When switched to Flat, the LED turns off, bubbles disappear, and the label changes to "FLAT."

---

## Signal Flow

```
Input Audio
     │
     ▼
┌─────────────────────────────────────┐
│  FLAVOR PROCESSOR                   │
│  (Active flavor's full DSP chain)   │
│                                     │
│  Saturation → Filters → Dynamics    │
│  (all at 4× oversampling if HQ=ON) │
└─────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────┐
│  AUTO-GAIN COMPENSATION             │
│  (RMS tracking + makeup gain)       │
│  Prevents dramatic volume shifts    │
│  when changing Fizz or flavors      │
└─────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────┐
│  OUTPUT GAIN                        │
│  (-12 dB to +12 dB)                │
└─────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────┐
│  SAFETY LIMITER                     │
│  (Prevents clipping / digital overs)│
└─────────────────────────────────────┘
     │
     ▼
Output Audio
```

**Auto-Gain Compensation** is worth noting: Carbonator tracks the input and output RMS levels and applies automatic makeup gain so that increasing Fizz doesn't dramatically change perceived volume. This lets you focus on the *character* of the effect without constantly adjusting the output knob. However, for critical A/B comparisons, use the Output Gain to manually level-match.

**Safety Limiter** catches any stray peaks that might exceed 0 dBFS, preventing digital clipping at the output. This means Carbonator will never hard-clip your DAW's output, even with extreme settings.

---

## Tips & Tricks

**Mixing:**
- **Cola at 30-40% Fizz** on a mix bus adds the "ran it through an SSL" vibe without any EQ or compression plugins. Subtle but transformative.
- **Cherry at 50-60%** on lead vocals is the sweet spot — forward, present, harmonically rich, never harsh.
- **Lemon-Lime at 20-30%** on acoustic guitars or overheads adds expensive-sounding sparkle without touching the body.

**Sound Design:**
- **Grape (Flat mode)** at high Fizz for instant lo-fi vinyl sampling vibes — crackle, rumble, and all.
- **Lemon-Lime (Flat mode)** with high Fizz for aggressive telephone/radio effect on vocals or synths.
- **Orange Cream (Flat mode)** at high Fizz with automation for that "swallowed by a subwoofer" riser effect.

**Automation:**
- **Orange Cream's Fizz** is built for automation — sweep it over 4-8 bars for classic filter transitions.
- **Grape's Fizz** automated slowly from 20% to 80% simulates a tape machine gradually deteriorating.
- Try automating the **Flavor Selector** itself for jarring creative transitions between textures.

**CPU Management:**
- Disable HQ mode when running many instances during composition. Re-enable for mixdown.
- Carbonated mode generally uses more CPU than Flat (more processing stages).

---

## System Requirements

- **macOS:** 10.13+ (Universal Binary — Apple Silicon and Intel)
- **Formats:** VST3, AU, AAX, Standalone
- **DAWs:** Any VST3/AU/AAX-compatible host (Pro Tools, Logic, Ableton, FL Studio, Reaper, Cubase, Studio One, etc.)
- **Sample Rates:** 44.1 kHz – 192 kHz

---

*Carbonator v2.1 — Made with love by MixedBySoda*
*"Pop the cap. Twist the fizz. Taste the sound."*
