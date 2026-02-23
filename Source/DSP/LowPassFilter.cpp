#include "LowPassFilter.h"
#include <cmath>

LowPassFilter::LowPassFilter (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts)
{
    using namespace ParameterIDs::Filter;

    // Get parameter pointers
    fizzParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter (fizzAmount.getParamID()));
    carbonatedParam = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter (carbonated.getParamID()));
}

void LowPassFilter::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    numChannels = static_cast<int>(spec.numChannels);

    // Create and prepare filters for each channel
    mainFilters.clear();
    flatFilters.clear();
    mainFilters.resize (numChannels);
    flatFilters.resize (numChannels);

    juce::dsp::ProcessSpec monoSpec;
    monoSpec.sampleRate = spec.sampleRate;
    monoSpec.maximumBlockSize = spec.maximumBlockSize;
    monoSpec.numChannels = 1;

    for (int ch = 0; ch < numChannels; ++ch)
    {
        // Main FIZZ-controlled filter
        mainFilters[ch].prepare (monoSpec);
        mainFilters[ch].reset();
        mainFilters[ch].setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        mainFilters[ch].setResonance (0.707f);  // Butterworth response (gentle)

        // FLAT mode filter (fixed at ~800 Hz)
        flatFilters[ch].prepare (monoSpec);
        flatFilters[ch].reset();
        flatFilters[ch].setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        flatFilters[ch].setCutoffFrequency (800.0f);
        flatFilters[ch].setResonance (0.707f);
    }

    // Initialize smoothed cutoff value (50ms ramp time)
    cutoffSmoothed.reset (sampleRate, 0.05);
    cutoffSmoothed.setCurrentAndTargetValue (fizzToCutoff (fizzParam->get()));
}

void LowPassFilter::process (juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& outputBlock = context.getOutputBlock();
    const auto numCh = outputBlock.getNumChannels();
    const auto numSamples = outputBlock.getNumSamples();

    // Update smoothed cutoff target
    cutoffSmoothed.setTargetValue (fizzToCutoff (fizzParam->get()));

    const bool isCarbonated = carbonatedParam->get();

    // Process each sample
    for (size_t i = 0; i < numSamples; ++i)
    {
        // Get smoothed cutoff for this sample
        const float cutoff = cutoffSmoothed.getNextValue();

        // Update and process main FIZZ filter for each channel
        for (size_t ch = 0; ch < numCh && ch < mainFilters.size(); ++ch)
        {
            mainFilters[ch].setCutoffFrequency (cutoff);

            auto* channelData = outputBlock.getChannelPointer (ch);
            channelData[i] = mainFilters[ch].processSample (0, channelData[i]);

            // If FLAT mode (carbonated OFF), apply additional low-pass
            if (!isCarbonated)
            {
                channelData[i] = flatFilters[ch].processSample (0, channelData[i]);
            }
        }
    }
}

void LowPassFilter::reset()
{
    for (auto& filter : mainFilters)
        filter.reset();

    for (auto& filter : flatFilters)
        filter.reset();
}

float LowPassFilter::fizzToCutoff (float fizzPercent)
{
    // Map FIZZ (0-100%) to cutoff frequency (300 Hz to 18 kHz)
    // Use exponential mapping for musical feel
    // 0% → 300 Hz (heavy underwater)
    // 50% → ~2500 Hz (mid-range)
    // 100% → 18000 Hz (full brightness)

    const float minFreq = 300.0f;
    const float maxFreq = 18000.0f;

    // Normalize to 0-1
    float normalized = fizzPercent / 100.0f;

    // Exponential mapping: freq = minFreq * (maxFreq/minFreq)^normalized
    float cutoff = minFreq * std::pow (maxFreq / minFreq, normalized);

    // Safety clamp
    return juce::jlimit (20.0f, 20000.0f, cutoff);
}
