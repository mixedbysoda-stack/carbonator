#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "LowPassFilter.h"
#include "FlavorProcessor.h"

/**
 * Main effects chain for Soda Filter
 * Signal flow: Input -> Low-Pass Filter -> Flavor Effect -> Output Gain -> Safety Limiter
 */
class EffectsChain
{
public:
    explicit EffectsChain (juce::AudioProcessorValueTreeState& apvts);

    /**
     * Prepare all effects for processing
     */
    void prepare (const juce::dsp::ProcessSpec& spec);

    /**
     * Process audio through all effects in series
     */
    void process (juce::dsp::ProcessContextReplacing<float>& context);

    /**
     * Reset all effects
     */
    void reset();

private:
    // Reference to APVTS
    juce::AudioProcessorValueTreeState& apvts;

    // Output gain control
    juce::dsp::Gain<float> outputGain;

    // Safety limiter (prevents clipping)
    juce::dsp::Limiter<float> outputLimiter;

    // Individual effect processors (in signal flow order)
    LowPassFilter lowPassFilter;
    FlavorProcessor flavorProcessor;

    // Bypass parameter
    juce::AudioParameterBool* bypassParam;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectsChain)
};
