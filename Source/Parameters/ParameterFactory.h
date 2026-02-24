#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

/**
 * Factory class for creating Soda Filter APVTS parameter layout
 * Defines 5 parameters: fizz, carbonated, flavor type, output gain, bypass
 */
class ParameterFactory
{
public:
    /**
     * Creates the complete parameter layout for APVTS
     */
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    // Helper functions for creating parameter groups
    static void addFilterParameters (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params);
    static void addFlavorParameters (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params);
    static void addGlobalParameters (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params);
};
