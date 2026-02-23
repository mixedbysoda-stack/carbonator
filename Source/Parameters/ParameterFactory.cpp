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
    // 0% = heavy underwater (~300 Hz cutoff)
    // 100% = full brightness (~18 kHz cutoff)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        fizzAmount,
        "Fizz",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
        50.0f,
        juce::AudioParameterFloatAttributes().withLabel ("%")
    ));

    // Carbonated Toggle
    // ON = fizzy (normal), OFF = flat (extra low-pass)
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        carbonated,
        "Carbonated",
        true  // Default to ON (fizzy)
    ));
}

void ParameterFactory::addFlavorParameters (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params)
{
    using namespace ParameterIDs::Flavor;

    // Flavor Type: Cherry, Grape, Dirty Soda
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        type,
        "Flavor",
        juce::StringArray { "Cherry", "Grape", "Dirty Soda" },
        0  // Default to Cherry
    ));

    // Flavor Intensity: 0% to 100%
    // Controls the wet/dry mix of the flavor effect
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        intensity,
        "Flavor Intensity",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
        50.0f,
        juce::AudioParameterFloatAttributes().withLabel ("%")
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
}
