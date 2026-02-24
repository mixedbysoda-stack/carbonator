#pragma once

#include <juce_dsp/juce_dsp.h>

/**
 * Shared oversampled saturation engine for Carbonator v2.0
 * Wraps juce::dsp::Oversampling with configurable transfer functions.
 * 4x oversampling with polyphase IIR half-band filters.
 */
class SaturationEngine
{
public:
    enum class CurveType
    {
        SoftClip,       // x/(1+|x|) — warm, even harmonics
        Tanh,           // std::tanh(x) — tube-like, odd harmonics
        AsymSoftClip,   // x/(1+|x|) with DC bias applied before — even harmonics via asymmetry
        WarmClip        // Cubic 1.5x - 0.5x^3 clamped — gentlest curve
    };

    struct Params
    {
        CurveType curve  = CurveType::SoftClip;
        float drive      = 1.0f;
        float dcBias     = 0.0f;    // Applied after drive, before waveshaping
        float outputGain = 1.0f;
        float mix        = 1.0f;    // 1.0 = fully wet, 0.0 = fully dry
    };

    SaturationEngine() = default;

    void prepare (const juce::dsp::ProcessSpec& spec);
    void process (juce::dsp::AudioBlock<float>& block, const Params& params);
    void reset();

    void setOversamplingEnabled (bool enabled) { oversamplingEnabled = enabled; }
    bool isOversamplingEnabled() const { return oversamplingEnabled; }
    float getLatencyInSamples() const;

private:
    static float applyWaveshaper (float sample, CurveType curve);

    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;
    bool oversamplingEnabled = true;
    int numChannels = 2;

    // Dry buffer for parallel mix
    juce::AudioBuffer<float> dryBuffer;
};
