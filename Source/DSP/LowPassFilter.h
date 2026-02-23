#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "Parameters/ParameterIDs.h"

/**
 * Low-pass filter for underwater/soda can effect
 * - FIZZ parameter: 0% = heavy underwater (~300 Hz), 100% = full brightness (~18 kHz)
 * - FLAT mode (carbonated OFF): adds additional low-pass at ~800 Hz
 */
class LowPassFilter
{
public:
    LowPassFilter (juce::AudioProcessorValueTreeState& apvts);

    void prepare (const juce::dsp::ProcessSpec& spec);
    void process (juce::dsp::ProcessContextReplacing<float>& context);
    void reset();

private:
    juce::AudioProcessorValueTreeState& apvts;

    // DSP components - one filter per channel for each stage
    std::vector<juce::dsp::StateVariableTPTFilter<float>> mainFilters;    // FIZZ-controlled
    std::vector<juce::dsp::StateVariableTPTFilter<float>> flatFilters;    // FLAT mode (~800 Hz)

    // Parameter attachments
    juce::AudioParameterFloat* fizzParam;
    juce::AudioParameterBool* carbonatedParam;

    // Smoothed values for parameter changes
    juce::SmoothedValue<float> cutoffSmoothed;

    double sampleRate = 44100.0;
    int numChannels = 2;

    // Helper function to map FIZZ (0-100%) to cutoff frequency
    float fizzToCutoff (float fizzPercent);
};
