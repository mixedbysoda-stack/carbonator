#include "EffectsChain.h"
#include "Parameters/ParameterIDs.h"

EffectsChain::EffectsChain (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts),
      lowPassFilter (apvts),
      flavorProcessor (apvts)
{
    using namespace ParameterIDs::Global;
    bypassParam = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter (bypass.getParamID()));
}

void EffectsChain::prepare (const juce::dsp::ProcessSpec& spec)
{
    // Prepare output gain control
    outputGain.prepare (spec);
    outputGain.setRampDurationSeconds (0.05); // 50ms ramp to avoid clicks

    // Prepare safety limiter
    outputLimiter.prepare (spec);
    outputLimiter.setThreshold (0.0f);      // 0 dBFS threshold
    outputLimiter.setRelease (50.0f);       // 50ms release time

    // Prepare all effects in signal flow order
    lowPassFilter.prepare (spec);
    flavorProcessor.prepare (spec);
}

void EffectsChain::process (juce::dsp::ProcessContextReplacing<float>& context)
{
    // Global bypass
    if (bypassParam->get())
        return;

    // Update output gain from parameter
    using namespace ParameterIDs;
    float outGainDb = apvts.getRawParameterValue (Global::outputGain.getParamID())->load();
    outputGain.setGainDecibels (outGainDb);

    // Process through effects chain in order:
    // Low-Pass Filter (FIZZ + FLAT) -> Flavor Effect -> Output Gain -> Safety Limiter

    lowPassFilter.process (context);
    flavorProcessor.process (context);
    outputGain.process (context);

    // Safety limiter prevents clipping
    outputLimiter.process (context);
}

void EffectsChain::reset()
{
    outputGain.reset();
    outputLimiter.reset();
    lowPassFilter.reset();
    flavorProcessor.reset();
}
