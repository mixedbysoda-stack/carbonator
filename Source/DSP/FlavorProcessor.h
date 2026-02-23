#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "Parameters/ParameterIDs.h"

/**
 * Flavor effect processor
 * - Cherry: Soft saturation/distortion
 * - Grape: Plate reverb
 * - Dirty Soda: Pitch shift (-12 semitones)
 */
class FlavorProcessor
{
public:
    FlavorProcessor (juce::AudioProcessorValueTreeState& apvts);

    void prepare (const juce::dsp::ProcessSpec& spec);
    void process (juce::dsp::ProcessContextReplacing<float>& context);
    void reset();

private:
    void updateReverbParameters();

    juce::AudioProcessorValueTreeState& apvts;

    // DSP components
    juce::dsp::Reverb reverb;                           // For Grape flavor
    juce::dsp::Reverb::Parameters reverbParams;

    // For pitch shifting (Dirty Soda) - using simple delay-based pitch shifter
    std::vector<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>> pitchDelays;
    std::vector<float> pitchPhases;  // Phase accumulators for each channel
    const float pitchShiftRatio = 0.5f;  // -12 semitones = 0.5x speed

    // Parameter attachments
    juce::AudioParameterChoice* flavorTypeParam;
    juce::AudioParameterFloat* intensityParam;

    // Smoothed values
    juce::SmoothedValue<float> intensitySmoothed;

    // Dry/wet buffer
    juce::AudioBuffer<float> dryBuffer;

    double sampleRate = 44100.0;
    int numChannels = 2;
};
