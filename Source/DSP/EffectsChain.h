#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "FlavorProcessor.h"

/**
 * Main effects chain for Carbonator v2.0
 * Signal flow: Input -> FlavorProcessor (Fizz morphing + Carbonated toggle)
 *              -> Auto-Gain Compensation -> Output Gain -> Safety Limiter
 */
class EffectsChain
{
public:
    explicit EffectsChain (juce::AudioProcessorValueTreeState& apvts);

    void prepare (const juce::dsp::ProcessSpec& spec);
    void process (juce::dsp::ProcessContextReplacing<float>& context);
    void reset();

    /** Get total processing latency (oversampling) */
    float getLatencyInSamples() const;

    /** Set quality mode (oversampling on/off) */
    void setQualityMode (bool hq);

private:
    juce::AudioProcessorValueTreeState& apvts;

    // Output gain control
    juce::dsp::Gain<float> outputGain;

    // Safety limiter (prevents clipping)
    juce::dsp::Limiter<float> outputLimiter;

    // Flavor effect processor (handles all DSP + Fizz morphing + Carbonated toggle)
    FlavorProcessor flavorProcessor;

    // Auto-gain compensation (smoothed)
    juce::SmoothedValue<float> autoGainCompensation;
    float inputRMS = 0.0f;
    float outputRMS = 0.0f;
    static constexpr float rmsAlpha = 0.01f;  // Exponential smoothing coefficient

    // Parameter pointers
    juce::AudioParameterBool* bypassParam;
    juce::AudioParameterFloat* fizzAmountParam;
    juce::AudioParameterBool* carbonatedParam;
    juce::AudioParameterBool* qualityModeParam;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectsChain)
};
