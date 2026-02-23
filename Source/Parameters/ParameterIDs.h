#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

/**
 * Soda Filter Parameter IDs
 * Simple 6-parameter plugin for underwater/soda can effect
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
        inline const juce::ParameterID type          { "flavorType",     1 };  // 0=Cherry, 1=Grape, 2=Dirty Soda
        inline const juce::ParameterID intensity     { "flavorIntensity", 1 };  // 0-100%: Effect strength
    }

    // Global Parameters
    namespace Global
    {
        inline const juce::ParameterID outputGain    { "outputGain",    1 };  // -12 to +12 dB
        inline const juce::ParameterID bypass        { "bypass",        1 };  // Bool: Global bypass
    }
}

/**
 * Enum for soda flavors
 */
enum class FlavorType
{
    Cherry = 0,      // Low-pass + Distortion
    Grape,           // Low-pass + Reverb
    DirtySoda        // Low-pass + Pitch Shift (-12 semitones)
};
