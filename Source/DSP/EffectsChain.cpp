#include "EffectsChain.h"
#include "Parameters/ParameterIDs.h"

EffectsChain::EffectsChain (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts),
      flavorProcessor (apvts)
{
    using namespace ParameterIDs;

    bypassParam = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter (Global::bypass.getParamID()));
    fizzAmountParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter (Filter::fizzAmount.getParamID()));
    carbonatedParam = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter (Filter::carbonated.getParamID()));
    qualityModeParam = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter (Global::qualityMode.getParamID()));
}

void EffectsChain::prepare (const juce::dsp::ProcessSpec& spec)
{
    // Output gain control
    outputGain.prepare (spec);
    outputGain.setRampDurationSeconds (0.05);

    // Safety limiter
    outputLimiter.prepare (spec);
    outputLimiter.setThreshold (0.0f);
    outputLimiter.setRelease (50.0f);

    // Flavor processor
    flavorProcessor.prepare (spec);

    // Auto-gain compensation (50ms ramp)
    autoGainCompensation.reset (spec.sampleRate, 0.05);
    autoGainCompensation.setCurrentAndTargetValue (1.0f);
    inputRMS = 0.0f;
    outputRMS = 0.0f;

#ifdef CARBONATOR_DEMO
    playDurationSamples = static_cast<int> (spec.sampleRate * 60.0);
    muteDurationSamples = static_cast<int> (spec.sampleRate * 10.0);
    sampleCounter = 0;
    muteCounter = 0;
    isMuted = false;
#endif
}

void EffectsChain::process (juce::dsp::ProcessContextReplacing<float>& context)
{
    // Global bypass
    if (bypassParam->get())
        return;

    auto& block = context.getOutputBlock();
    const auto nChannels = block.getNumChannels();
    const auto nSamples = block.getNumSamples();

    // 1. Measure input RMS (per-channel average, exponential smoothing)
    {
        float sumSq = 0.0f;
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            const auto* data = block.getChannelPointer (ch);
            for (size_t i = 0; i < nSamples; ++i)
                sumSq += data[i] * data[i];
        }
        float blockRMS = std::sqrt (sumSq / static_cast<float> (nChannels * nSamples + 1));
        inputRMS += rmsAlpha * (blockRMS - inputRMS);
    }

    // 2. Pass Fizz amount (0-1), Carbonated state, and quality mode to FlavorProcessor
    float fizzNormalized = fizzAmountParam->get() / 100.0f;
    flavorProcessor.setFizzAmount (fizzNormalized);
    flavorProcessor.setCarbonated (carbonatedParam->get());
    flavorProcessor.setQualityMode (qualityModeParam->get());

    // 3. Process through flavor DSP
    flavorProcessor.process (context);

#ifndef CARBONATOR_DEMO
    // License check #2 (anti-patch scatter) — clears audio if unlicensed
    if (licenseFlag != nullptr && ! licenseFlag->load (std::memory_order_relaxed))
    {
        for (size_t ch = 0; ch < nChannels; ++ch)
            juce::FloatVectorOperations::clear (block.getChannelPointer (ch), static_cast<int> (nSamples));
        return;
    }
#endif

    // 4. Measure output RMS
    {
        float sumSq = 0.0f;
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            const auto* data = block.getChannelPointer (ch);
            for (size_t i = 0; i < nSamples; ++i)
                sumSq += data[i] * data[i];
        }
        float blockRMS = std::sqrt (sumSq / static_cast<float> (nChannels * nSamples + 1));
        outputRMS += rmsAlpha * (blockRMS - outputRMS);
    }

    // 5. Compute and apply auto-gain correction (clamped to +/-12dB)
    if (outputRMS > 1e-6f && inputRMS > 1e-6f)
    {
        float correction = inputRMS / outputRMS;
        // Clamp to +/-12dB range
        constexpr float maxGain = 3.981f;  // +12dB
        constexpr float minGain = 0.251f;  // -12dB
        correction = juce::jlimit (minGain, maxGain, correction);
        autoGainCompensation.setTargetValue (correction);
    }

    // Apply smoothed auto-gain
    for (size_t i = 0; i < nSamples; ++i)
    {
        float gain = autoGainCompensation.getNextValue();
        for (size_t ch = 0; ch < nChannels; ++ch)
            block.getChannelPointer (ch)[i] *= gain;
    }

    // 6. User output gain + safety limiter
    using namespace ParameterIDs;
    float outGainDb = apvts.getRawParameterValue (Global::outputGain.getParamID())->load();
    outputGain.setGainDecibels (outGainDb);

    outputGain.process (context);
    outputLimiter.process (context);

#ifdef CARBONATOR_DEMO
    // Demo mute cycle: 60s play, 10s mute with smooth crossfade
    for (size_t i = 0; i < nSamples; ++i)
    {
        float gain = 1.0f;

        if (! isMuted)
        {
            ++sampleCounter;
            if (sampleCounter >= playDurationSamples)
            {
                // Entering mute phase
                isMuted = true;
                muteCounter = 0;
            }
            else if (sampleCounter > playDurationSamples - fadeSamples)
            {
                // Fade out: last fadeSamples of play phase
                int fadePos = sampleCounter - (playDurationSamples - fadeSamples);
                gain = 1.0f - static_cast<float> (fadePos) / static_cast<float> (fadeSamples);
            }
        }
        else
        {
            ++muteCounter;
            if (muteCounter >= muteDurationSamples)
            {
                // Exiting mute phase
                isMuted = false;
                sampleCounter = 0;
            }
            else if (muteCounter > muteDurationSamples - fadeSamples)
            {
                // Fade in: last fadeSamples of mute phase
                int fadePos = muteCounter - (muteDurationSamples - fadeSamples);
                gain = static_cast<float> (fadePos) / static_cast<float> (fadeSamples);
            }
            else
            {
                gain = 0.0f;
            }
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
            block.getChannelPointer (ch)[i] *= gain;
    }
#endif
}

void EffectsChain::reset()
{
    outputGain.reset();
    outputLimiter.reset();
    flavorProcessor.reset();
    autoGainCompensation.setCurrentAndTargetValue (1.0f);
    inputRMS = 0.0f;
    outputRMS = 0.0f;

#ifdef CARBONATOR_DEMO
    sampleCounter = 0;
    muteCounter = 0;
    isMuted = false;
#endif
}

float EffectsChain::getLatencyInSamples() const
{
    return flavorProcessor.getLatencyInSamples();
}

void EffectsChain::setQualityMode (bool hq)
{
    flavorProcessor.setQualityMode (hq);
}
