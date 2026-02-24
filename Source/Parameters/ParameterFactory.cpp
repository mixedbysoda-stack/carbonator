#include "ParameterFactory.h"
#include "ParameterIDs.h"

juce::AudioProcessorValueTreeState::ParameterLayout ParameterFactory::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    addFilterParameters (params);
    addFlavorParameters (params);
    addGlobalParameters (params);

    return { params.begin(), params.end() };
}

void ParameterFactory::addFilterParameters (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params)
{
    using namespace ParameterIDs::Filter;

    // Fizz Amount: 0% to 100%
    // Multi-parameter morph controller (not a simple dry/wet)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        fizzAmount,
        "Fizz",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
        50.0f,
        juce::AudioParameterFloatAttributes().withLabel ("%")
    ));

    // Carbonated Toggle
    // ON = normal mode, OFF = per-flavor alternate mode
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        carbonated,
        "Carbonated",
        true  // Default to ON (fizzy)
    ));
}

void ParameterFactory::addFlavorParameters (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params)
{
    using namespace ParameterIDs::Flavor;

    // Flavor Type: Cola, Cherry, Grape, Lemon-Lime, Orange Cream
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        type,
        "Flavor",
        juce::StringArray { "Cola", "Cherry", "Grape", "Lemon-Lime", "Orange Cream" },
        0  // Default to Cola
    ));
}

void ParameterFactory::addGlobalParameters (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params)
{
    using namespace ParameterIDs::Global;

    // Output Gain: -12 dB to +12 dB
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        outputGain,
        "Output Gain",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")
    ));

    // Global Bypass
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        bypass,
        "Bypass",
        false
    ));

    // HQ Mode (4x oversampling on/off)
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        qualityMode,
        "HQ Mode",
        true  // Default to ON (high quality)
    ));
}
