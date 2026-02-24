#include "SaturationEngine.h"
#include <cmath>

void SaturationEngine::prepare (const juce::dsp::ProcessSpec& spec)
{
    numChannels = static_cast<int> (spec.numChannels);

    // 4x oversampling (2 stages) with polyphase IIR half-band filters
    oversampling = std::make_unique<juce::dsp::Oversampling<float>> (
        spec.numChannels, 2,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);

    oversampling->initProcessing (spec.maximumBlockSize);

    dryBuffer.setSize (numChannels, static_cast<int> (spec.maximumBlockSize));
}

void SaturationEngine::process (juce::dsp::AudioBlock<float>& block, const Params& params)
{
    const auto nChannels = block.getNumChannels();
    const auto nSamples = block.getNumSamples();

    // Save dry signal if parallel mix needed
    const bool needsDryMix = params.mix < 0.999f;
    if (needsDryMix)
    {
        for (size_t ch = 0; ch < nChannels; ++ch)
            dryBuffer.copyFrom (static_cast<int> (ch), 0,
                                block.getChannelPointer (ch),
                                static_cast<int> (nSamples));
    }

    // Lambda for the inner saturation loop
    auto processBlock = [&] (juce::dsp::AudioBlock<float>& audioBlock)
    {
        const auto numSamples = audioBlock.getNumSamples();
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            auto* data = audioBlock.getChannelPointer (ch);
            for (size_t i = 0; i < numSamples; ++i)
            {
                float sample = data[i] * params.drive + params.dcBias;
                sample = applyWaveshaper (sample, params.curve);
                data[i] = sample * params.outputGain;
            }
        }
    };

    if (oversamplingEnabled && oversampling != nullptr)
    {
        auto oversampledBlock = oversampling->processSamplesUp (block);
        processBlock (oversampledBlock);
        oversampling->processSamplesDown (block);
    }
    else
    {
        processBlock (block);
    }

    // Parallel dry/wet mix
    if (needsDryMix)
    {
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            auto* wetData = block.getChannelPointer (ch);
            const auto* dryData = dryBuffer.getReadPointer (static_cast<int> (ch));
            for (size_t i = 0; i < nSamples; ++i)
                wetData[i] = dryData[i] * (1.0f - params.mix) + wetData[i] * params.mix;
        }
    }
}

void SaturationEngine::reset()
{
    if (oversampling != nullptr)
        oversampling->reset();
}

float SaturationEngine::getLatencyInSamples() const
{
    if (oversamplingEnabled && oversampling != nullptr)
        return oversampling->getLatencyInSamples();
    return 0.0f;
}

float SaturationEngine::applyWaveshaper (float sample, CurveType curve)
{
    switch (curve)
    {
        case CurveType::SoftClip:
        case CurveType::AsymSoftClip:
            // x/(1+|x|) — AsymSoftClip gets its asymmetry from the dcBias applied before
            return sample / (1.0f + std::abs (sample));

        case CurveType::Tanh:
            return std::tanh (sample);

        case CurveType::WarmClip:
        {
            float clamped = juce::jlimit (-1.0f, 1.0f, sample);
            return 1.5f * clamped - 0.5f * clamped * clamped * clamped;
        }
    }
    return sample;
}
