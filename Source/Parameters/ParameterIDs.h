#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

/**
 * Carbonator v2.0 Parameter IDs
 * 5 parameters: fizz, carbonated, flavor type, output gain, bypass
 */
namespace ParameterIDs
{
    // Filter Parameters
    namespace Filter
    {
        inline const juce::ParameterID fizzAmount    { "fizzAmount",    1 };  // 0-100%: Main control
        inline const juce::ParameterID carbonated    { "carbonated",    1 };  // Bool: On=fizzy, Off=flat
    }

    // Flavor Parameters
    namespace Flavor
    {
        inline const juce::ParameterID type          { "flavorType",     1 };  // 0=Cola, 1=Cherry, 2=Grape, 3=LemonLime, 4=OrangeCream
    }

    // Global Parameters
    namespace Global
    {
        inline const juce::ParameterID outputGain    { "outputGain",    1 };  // -12 to +12 dB
        inline const juce::ParameterID bypass        { "bypass",        1 };  // Bool: Global bypass
        inline const juce::ParameterID qualityMode   { "qualityMode",   1 };  // Bool: HQ mode (4x oversampling)
    }
}

/**
 * Enum for soda flavors (v2.0)
 */
enum class FlavorType
{
    Cola = 0,        // Analog Console Warmth
    Cherry,          // Sweet Vocal Presence
    Grape,           // Lo-Fi Tape Texture
    LemonLime,       // Crisp Exciter
    OrangeCream      // Stereo Width + Warmth
};
