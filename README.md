# SODA FILTER

**Vintage Carbonated Audio Plugin**

A JUCE audio plugin that simulates the sound of "a microphone in a bucket of soda" - an underwater low-pass effect with unique flavor variations.

---

## Features

### Core Effect
- **Base Low-Pass Filter**: Creates an underwater/submerged audio effect
- **FIZZ Control (0-100%)**: Main parameter that controls high-frequency restoration
  - 0% = Heavy underwater (~300 Hz cutoff) - full submersion
  - 100% = Full brightness (~18 kHz cutoff) - restored clarity
  - Smooth exponential curve for musical response

### Carbonation Control
- **CARBONATED Toggle**:
  - ON (fizzy) = Normal filter response
  - OFF (flat) = Additional low-pass at ~800 Hz for extra muffled "flat soda" effect

### Flavor Variations
Three unique effects that combine with the base underwater filter:

1. **CHERRY** - Low-pass + Soft Saturation
   - Adds warmth and harmonic distortion
   - Vintage tape-like character

2. **GRAPE** - Low-pass + Plate Reverb
   - Adds vintage reverb space
   - Creates underwater ambience

3. **DIRTY SODA** - Low-pass + Pitch Shift
   - Shifts pitch down by 12 semitones (one octave)
   - Lo-fi pitch shifting for creative effects

### Additional Controls
- **Flavor Intensity (0-100%)**: Controls wet/dry mix of the selected flavor effect
- **Output Gain (-12 to +12 dB)**: Master output level control
- **Bypass**: Global plugin bypass

---

## Technical Details

### DSP Signal Chain
```
Input Audio
    ↓
[Base Low-Pass Filter]
    • FIZZ-controlled cutoff (300 Hz - 18 kHz)
    • Smooth exponential mapping
    • Butterworth response (12 dB/octave)
    ↓
[FLAT Mode Low-Pass] (optional)
    • Additional stage at 800 Hz when carbonated is OFF
    ↓
[Flavor Effect Processor]
    • Cherry: tanh waveshaping (soft saturation)
    • Grape: JUCE reverb (vintage plate settings)
    • Dirty Soda: Delay-based pitch shifter (-12 semitones)
    ↓
[Output Gain + Safety Limiter]
    ↓
Output Audio
```

### Parameter Count
**6 parameters total** - simple, focused design:
- fizzAmount (float, 0-100%)
- carbonated (bool)
- flavorType (choice: 0=Cherry, 1=Grape, 2=Dirty Soda)
- flavorIntensity (float, 0-100%)
- outputGain (float, -12 to +12 dB)
- bypass (bool)

---

## Installation

The plugin was built in the following formats:

### macOS
- **VST3**: `~/Library/Audio/Plug-Ins/VST3/Soda Filter.vst3`
- **AU**: `~/Library/Audio/Plug-Ins/Components/Soda Filter.component`
- **Standalone**: `~/Desktop/SodaFilter/build/SodaFilter_artefacts/Release/Standalone/Soda Filter.app`

### DAW Compatibility
- Logic Pro (AU)
- Ableton Live (VST3/AU)
- FL Studio (VST3)
- Pro Tools (requires AAX build - see below)
- Any DAW supporting VST3 or AU formats

---

## Building from Source

### Requirements
- macOS 10.13+ (or Windows/Linux with appropriate toolchain)
- CMake 3.22+
- C++20 compiler (Xcode 12+ on macOS)
- JUCE framework (included as submodule)

### Build Commands
```bash
cd ~/Desktop/SodaFilter

# Initialize JUCE submodule
git submodule update --init --recursive

# Configure CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build all formats
cmake --build build --config Release

# Plugins will be automatically copied to system plugin folders
```

### Build Artifacts
After building, you'll find:
- `build/SodaFilter_artefacts/Release/VST3/Soda Filter.vst3`
- `build/SodaFilter_artefacts/Release/AU/Soda Filter.component`
- `build/SodaFilter_artefacts/Release/Standalone/Soda Filter.app`

---

## UI Design

### Color Palette (Retro Soda Can Aesthetic)
- **Primary Red**: `#ff3b30` - Borders, accents, text highlights
- **Cream Background**: `#fff8f0` - Main background
- **Dark Navy**: `#2c2c2c` - Knob body, text
- **Mint Accent**: `#4cd964` - Fresh accent color

### Layout
```
┌──────────────────────────────────────────┐
│  ╔═══════════════════════════════════╗  │ <- Red border
│  ║      SODA FILTER                  ║  │
│  ║  VINTAGE CARBONATED AUDIO         ║  │
│  ║                                    ║  │
│  ║         ╭─────────╮                ║  │
│  ║         │  FIZZ   │                ║  │ <- Large dark knob
│  ║         ╰─────────╯                ║  │
│  ║            50%                     ║  │
│  ║                                    ║  │
│  ║ ───────────────────────────────── ║  │
│  ║  FLAVOR    │    CARBONATED  [ON]  ║  │ <- Footer controls
│  ║  CHERRY    │                      ║  │
│  ╚═══════════════════════════════════╝  │
└──────────────────────────────────────────┘
```

### Window Sizing
- **Default**: 420 x 560 pixels (~3:4 aspect ratio, vertical like soda can)
- **Minimum**: 350 x 450 pixels
- **Maximum**: 600 x 800 pixels
- Fully resizable with aspect ratio constraint

---

## Usage Tips

### Getting Started
1. **Set FIZZ to 0%** - Full underwater effect (heavy low-pass)
2. **Gradually increase FIZZ** - Brings back high frequencies
3. **Try different FLAVORS** - Each adds unique character
4. **Adjust Intensity** - Control how much flavor effect is applied
5. **Toggle CARBONATED OFF** - For extra muffled "flat soda" sound

### Creative Applications
- **Underwater Vocals**: Low FIZZ (10-30%) + Grape flavor
- **Lo-Fi Drums**: Mid FIZZ (40-60%) + Cherry flavor
- **Bass Drops**: Automate FIZZ from 0% to 100% over time
- **Dirty Bass**: Low FIZZ + Dirty Soda flavor
- **Vintage Radio**: Low FIZZ + FLAT mode + Cherry flavor
- **Ambient Textures**: Low FIZZ + Grape flavor (high intensity)

### Performance
- **CPU Usage**: <15% on modern systems
- **Latency**: Zero-latency (no look-ahead required)
- **Sample Rates**: Supports 44.1 kHz - 192 kHz

---

## Project Structure

```
~/Desktop/SodaFilter/
├── CMakeLists.txt                    # Build configuration
├── JUCE/                             # JUCE framework (submodule)
├── Source/
│   ├── PluginProcessor.h/.cpp        # Main audio processor
│   ├── PluginEditor.h/.cpp           # Main UI editor
│   │
│   ├── Parameters/
│   │   ├── ParameterIDs.h            # Parameter namespace definitions
│   │   ├── ParameterFactory.h/.cpp   # APVTS parameter layout
│   │
│   ├── DSP/
│   │   ├── EffectsChain.h/.cpp       # Main processing chain
│   │   ├── LowPassFilter.h/.cpp      # Base underwater filter
│   │   └── FlavorProcessor.h/.cpp    # Cherry/Grape/Dirty Soda effects
│   │
│   └── UI/
│       ├── LookAndFeel/
│       │   ├── ColorScheme.h         # Soda color palette
│       │   ├── SodaLookAndFeel.h/.cpp    # Custom retro rendering
│       ├── Components/
│       │   ├── FizzKnob.h/.cpp           # Large central knob
│       │   ├── FlavorSelector.h/.cpp     # 3-way flavor picker
│       │   └── CarbonatedToggle.h/.cpp   # On/Flat toggle
│       └── SodaPanel.h/.cpp              # Main UI container
```

---

## Version History

### v1.0.0 (2026-02-22) - Initial Release
- ✅ Core underwater low-pass filter with FIZZ control
- ✅ FLAT mode (carbonated toggle) for extra filtering
- ✅ Three flavor effects: Cherry (distortion), Grape (reverb), Dirty Soda (pitch shift)
- ✅ Retro soda can UI aesthetic
- ✅ VST3, AU, and Standalone formats
- ✅ Fully resizable window
- ✅ Zero-latency processing
- ✅ Safety limiter to prevent clipping

---

## Credits

**Developer**: Mixed By Soda
**Framework**: JUCE 8.x
**Plugin Code**: `Soda`
**Manufacturer Code**: `Mxbs`

---

## License

[Your license here - MIT, GPL, proprietary, etc.]

---

## Support

For bug reports, feature requests, or questions:
- GitHub Issues: [Your repo URL]
- Email: [Your email]
- Website: [Your website]

---

**Enjoy your vintage carbonated audio experience! 🥤**
