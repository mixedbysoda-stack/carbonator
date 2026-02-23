#include "FlavorProcessor.h"
#include <cmath>

FlavorProcessor::FlavorProcessor (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts)
{
    using namespace ParameterIDs::Flavor;

    // Get parameter pointers
    flavorTypeParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter (type.getParamID()));
    intensityParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter (intensity.getParamID()));
}

void FlavorProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    numChannels = static_cast<int>(spec.numChannels);

    // Prepare reverb (for Grape flavor)
    reverb.prepare (spec);
    updateReverbParameters();

    // Prepare pitch shifter delays (for Dirty Soda flavor)
    pitchDelays.clear();
    pitchPhases.clear();
    pitchDelays.resize (numChannels);
    pitchPhases.resize (numChannels, 0.0f);

    for (auto& delay : pitchDelays)
    {
        delay.prepare (spec);
        delay.setMaximumDelayInSamples (static_cast<int>(sampleRate * 0.1));  // 100ms buffer
        delay.reset();
    }

    // Initialize smoothed intensity value (50ms ramp time)
    intensitySmoothed.reset (sampleRate, 0.05);
    intensitySmoothed.setCurrentAndTargetValue (intensityParam->get() / 100.0f);

    // Prepare dry buffer
    dryBuffer.setSize (numChannels, static_cast<int>(spec.maximumBlockSize));
}

void FlavorProcessor::process (juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& outputBlock = context.getOutputBlock();
    const auto numCh = outputBlock.getNumChannels();
    const auto numSamples = outputBlock.getNumSamples();

    // Update smoothed intensity target
    intensitySmoothed.setTargetValue (intensityParam->get() / 100.0f);

    // Store dry signal for wet/dry mix
    dryBuffer.setSize (static_cast<int>(numCh), static_cast<int>(numSamples), false, false, true);
    for (size_t ch = 0; ch < numCh; ++ch)
        dryBuffer.copyFrom (static_cast<int>(ch), 0, outputBlock.getChannelPointer (ch), static_cast<int>(numSamples));

    auto flavorType = static_cast<FlavorType>(flavorTypeParam->getIndex());

    // Apply flavor effect
    switch (flavorType)
    {
        case FlavorType::Cherry:
        {
            // Soft saturation using tanh waveshaping
            for (size_t ch = 0; ch < numCh; ++ch)
            {
                auto* channelData = outputBlock.getChannelPointer (ch);
                for (size_t i = 0; i < numSamples; ++i)
                {
                    // Drive the signal before saturation (3x gain for noticeable effect)
                    float driven = channelData[i] * 3.0f;
                    // Apply tanh saturation
                    channelData[i] = std::tanh (driven);
                }
            }
            break;
        }

        case FlavorType::Grape:
        {
            // Reverb effect
            updateReverbParameters();

            // Process reverb
            juce::dsp::AudioBlock<float> block (outputBlock);
            juce::dsp::ProcessContextReplacing<float> reverbContext (block);
            reverb.process (reverbContext);
            break;
        }

        case FlavorType::DirtySoda:
        {
            // Simple pitch shift using time-stretching
            // This creates a "dirty" lo-fi pitch shift effect
            const float delayTime = 0.02f;  // 20ms window
            const int delaySamples = static_cast<int>(delayTime * sampleRate);
            const float readSpeed = pitchShiftRatio;  // 0.5 = down one octave

            for (size_t ch = 0; ch < numCh && ch < pitchDelays.size(); ++ch)
            {
                auto* channelData = outputBlock.getChannelPointer (ch);

                for (size_t i = 0; i < numSamples; ++i)
                {
                    // Write input to delay line
                    pitchDelays[ch].pushSample (0, channelData[i]);

                    // Read from delay line with variable speed
                    float delayOffset = delaySamples + pitchPhases[ch];
                    channelData[i] = pitchDelays[ch].popSample (0, delayOffset, true);

                    // Update phase accumulator
                    pitchPhases[ch] += (1.0f - readSpeed);

                    // Wrap phase to create seamless pitch shift
                    if (pitchPhases[ch] >= delaySamples * 0.5f)
                        pitchPhases[ch] -= delaySamples * 0.5f;
                }
            }
            break;
        }
    }

    // Apply wet/dry mix based on intensity
    for (size_t ch = 0; ch < numCh; ++ch)
    {
        auto* wetData = outputBlock.getChannelPointer (ch);
        const auto* dryData = dryBuffer.getReadPointer (static_cast<int>(ch));

        if (ch == 0)
        {
            for (size_t i = 0; i < numSamples; ++i)
            {
                const float wetMix = intensitySmoothed.getNextValue();
                const float dryMix = 1.0f - wetMix;
                wetData[i] = wetData[i] * wetMix + dryData[i] * dryMix;
            }
        }
        else
        {
            const float wetMix = intensitySmoothed.getCurrentValue();
            const float dryMix = 1.0f - wetMix;
            for (size_t i = 0; i < numSamples; ++i)
                wetData[i] = wetData[i] * wetMix + dryData[i] * dryMix;
        }
    }
}

void FlavorProcessor::reset()
{
    reverb.reset();

    for (auto& delay : pitchDelays)
        delay.reset();

    for (auto& phase : pitchPhases)
        phase = 0.0f;
}

void FlavorProcessor::updateReverbParameters()
{
    // Vintage plate reverb settings for Grape flavor
    reverbParams.roomSize = 0.7f;      // Medium-large room
    reverbParams.damping = 0.6f;       // Warm, vintage damping
    reverbParams.wetLevel = 1.0f;      // Full wet (we handle mix externally)
    reverbParams.dryLevel = 0.0f;      // No dry (we handle mix externally)
    reverbParams.width = 1.0f;         // Full stereo width
    reverbParams.freezeMode = 0.0f;    // No freeze

    reverb.setParameters (reverbParams);
}
