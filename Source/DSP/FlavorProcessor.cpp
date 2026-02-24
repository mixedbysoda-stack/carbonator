#include "FlavorProcessor.h"
#include "FizzCurves.h"
#include <cmath>

// Max delay buffer size for modulated delays (200ms at 192kHz)
static constexpr int kMaxDelayBufferSize = 38400;
// Chorus delay buffer (50ms)
static constexpr int kChorusBufferSize = 9600;

FlavorProcessor::FlavorProcessor (juce::AudioProcessorValueTreeState& apvts)
{
    using namespace ParameterIDs::Flavor;
    flavorTypeParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter (type.getParamID()));
}

void FlavorProcessor::setFizzAmount (float newFizz)
{
    smoothedFizz.setTargetValue (juce::jlimit (0.0f, 1.0f, newFizz));
}

void FlavorProcessor::setCarbonated (bool isCarbonated)
{
    carbonatedState = isCarbonated;
}

void FlavorProcessor::setQualityMode (bool hq)
{
    saturationEngine.setOversamplingEnabled (hq);
}

float FlavorProcessor::getLatencyInSamples() const
{
    return saturationEngine.getLatencyInSamples();
}

void FlavorProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    numChannels = static_cast<int>(spec.numChannels);
    blockSize = static_cast<int>(spec.maximumBlockSize);

    // SmoothedValue for Fizz (~20ms ramp)
    smoothedFizz.reset (sampleRate, 0.02);
    smoothedFizz.setCurrentAndTargetValue (0.5f);

    // Shared saturation engine
    saturationEngine.prepare (spec);

    // ─── COLA ───────────────────────────────────────────────────
    colaCompressor.prepare (spec);
    colaDCBlocker.prepare (spec);
    colaDCBlocker.setType (juce::dsp::StateVariableTPTFilterType::highpass);
    colaDCBlocker.setCutoffFrequency (5.0f);
    colaLowShelf.prepare (spec);
    colaHighShelf.prepare (spec);

    // ─── CHERRY ─────────────────────────────────────────────────
    cherryDeHarsh.prepare (spec);
    cherryPresence.prepare (spec);
    cherryAirShelf.prepare (spec);
    cherryChorusPhase = 0.0f;
    cherryChorusDelayBuffer.setSize (numChannels, kChorusBufferSize);
    cherryChorusDelayBuffer.clear();
    cherryChorusWritePos = 0;

    // ─── GRAPE ──────────────────────────────────────────────────
    grapeDCBlocker.prepare (spec);
    grapeDCBlocker.setType (juce::dsp::StateVariableTPTFilterType::highpass);
    grapeDCBlocker.setCutoffFrequency (5.0f);
    grapeTapeLP.prepare (spec);
    grapeTapeLP.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    grapeDelayBuffer.setSize (numChannels, kMaxDelayBufferSize);
    grapeDelayBuffer.clear();
    grapeDelayWritePos = 0;
    grapeWowPhase = 0.0f;
    grapeFlutterPhase = 0.0f;
    grapeVinylRumbleLP.prepare (spec);
    grapeVinylRumbleLP.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    grapeVinylRumbleLP.setCutoffFrequency (40.0f);

    // ─── LEMON-LIME ─────────────────────────────────────────────
    lemonLowPass1.prepare (spec);
    lemonLowPass1.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    lemonLowPass2.prepare (spec);
    lemonLowPass2.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    lemonHighPass1.prepare (spec);
    lemonHighPass1.setType (juce::dsp::StateVariableTPTFilterType::highpass);
    lemonHighPass2.prepare (spec);
    lemonHighPass2.setType (juce::dsp::StateVariableTPTFilterType::highpass);
    lemonLowBandBuffer.setSize (numChannels, blockSize);
    lemonHFCompressor.prepare (spec);
    lemonPresence.prepare (spec);
    lemonAirShelf.prepare (spec);
    lemonTeleBandpass.prepare (spec);
    lemonTeleHighCut.prepare (spec);

    // ─── ORANGE CREAM ───────────────────────────────────────────
    orangeMidCompressor.prepare (spec);
    orangeDetuneDelayL.setSize (1, kMaxDelayBufferSize);
    orangeDetuneDelayL.clear();
    orangeDetuneDelayR.setSize (1, kMaxDelayBufferSize);
    orangeDetuneDelayR.clear();
    orangeDetuneWritePosL = 0;
    orangeDetuneWritePosR = 0;
    orangeDetunePitchPhaseL = 0.0f;
    orangeDetunePitchPhaseR = 0.0f;
    orangeWarmShelf.prepare (spec);
}

void FlavorProcessor::process (juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& outputBlock = context.getOutputBlock();
    juce::dsp::AudioBlock<float> block (outputBlock);

    auto flavorType = static_cast<FlavorType>(flavorTypeParam->getIndex());

    if (carbonatedState)
    {
        switch (flavorType)
        {
            case FlavorType::Cola:        processCola (block); break;
            case FlavorType::Cherry:      processCherry (block); break;
            case FlavorType::Grape:       processGrape (block); break;
            case FlavorType::LemonLime:   processLemonLime (block); break;
            case FlavorType::OrangeCream: processOrangeCream (block); break;
        }
    }
    else
    {
        // FLAT (Carbonated OFF) — per-flavor alternate mode
        switch (flavorType)
        {
            case FlavorType::Cola:        processCola (block); processColaFlat (block); break;
            case FlavorType::Cherry:      processCherry (block); processCherryFlat (block); break;
            case FlavorType::Grape:       processGrapeFlat (block); break;
            case FlavorType::LemonLime:   processLemonLime (block); processLemonLimeFlat (block); break;
            case FlavorType::OrangeCream: processOrangeCreamFlat (block); break;
        }
    }
}

void FlavorProcessor::reset()
{
    saturationEngine.reset();

    colaCompressor.reset();
    colaDCBlocker.reset();
    colaLowShelf.reset();
    colaHighShelf.reset();

    cherryDeHarsh.reset();
    cherryPresence.reset();
    cherryAirShelf.reset();
    cherryChorusDelayBuffer.clear();
    cherryChorusWritePos = 0;
    cherryChorusPhase = 0.0f;

    grapeDCBlocker.reset();
    grapeTapeLP.reset();
    grapeDelayBuffer.clear();
    grapeDelayWritePos = 0;
    grapeWowPhase = 0.0f;
    grapeFlutterPhase = 0.0f;
    grapeVinylRumbleLP.reset();

    lemonLowPass1.reset();
    lemonLowPass2.reset();
    lemonHighPass1.reset();
    lemonHighPass2.reset();
    lemonHFCompressor.reset();
    lemonPresence.reset();
    lemonAirShelf.reset();
    lemonTeleBandpass.reset();
    lemonTeleHighCut.reset();

    orangeMidCompressor.reset();
    orangeDetuneDelayL.clear();
    orangeDetuneDelayR.clear();
    orangeDetuneWritePosL = 0;
    orangeDetuneWritePosR = 0;
    orangeDetunePitchPhaseL = 0.0f;
    orangeDetunePitchPhaseR = 0.0f;
    orangeWarmShelf.reset();
}

// =============================================================================
// COLA — Analog Console Warmth
// =============================================================================
void FlavorProcessor::processCola (juce::dsp::AudioBlock<float>& block)
{
    const auto nSamples = block.getNumSamples();
    float fizz = smoothedFizz.getCurrentValue();

    // Fizz-morphed parameters (shaped curves)
    float drive      = FizzCurves::exponential (fizz, 1.0f, 4.0f, 2.5f);
    float compRatio  = FizzCurves::exponential (fizz, 1.5f, 6.0f, 2.0f);
    float compThresh = FizzCurves::linear (fizz, -6.0f, -30.0f);
    float lowGainDb  = FizzCurves::logarithmic (fizz, 0.0f, 3.5f, 1.8f);
    float highGainDb = FizzCurves::logarithmic (fizz, 0.0f, -3.0f, 1.8f);

    // Advance smoothed fizz for the block
    for (size_t i = 0; i < nSamples; ++i)
        smoothedFizz.getNextValue();

    // Oversampled asymmetric soft clip via SaturationEngine
    SaturationEngine::Params satParams;
    satParams.curve = SaturationEngine::CurveType::AsymSoftClip;
    satParams.drive = drive;
    satParams.dcBias = 0.1f;
    saturationEngine.process (block, satParams);

    // DC Blocker (HPF @ 5Hz)
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        colaDCBlocker.process (ctx);
    }

    // Compressor
    colaCompressor.setRatio (compRatio);
    colaCompressor.setThreshold (compThresh);
    colaCompressor.setAttack (10.0f);
    colaCompressor.setRelease (100.0f);
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        colaCompressor.process (ctx);
    }

    // Tilt EQ: low shelf + high shelf
    *colaLowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf (
        sampleRate, 200.0f, 0.707f, juce::Decibels::decibelsToGain (lowGainDb));
    *colaHighShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf (
        sampleRate, 8000.0f, 0.707f, juce::Decibels::decibelsToGain (highGainDb));

    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        colaLowShelf.process (ctx);
    }
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        colaHighShelf.process (ctx);
    }
    // No static makeup gain — auto-gain compensation handles this in EffectsChain
}

void FlavorProcessor::processColaFlat (juce::dsp::AudioBlock<float>& block)
{
    // FLAT: extra tape saturation layer
    // Runs inline (not through SaturationEngine) to avoid double-pumping the
    // oversampling filters — the signal is already band-limited from processCola's
    // oversampled saturation pass.
    const auto nChannels = block.getNumChannels();
    const auto nSamples = block.getNumSamples();
    float fizz = smoothedFizz.getCurrentValue();
    float tapeDrive = FizzCurves::exponential (fizz, 1.0f, 3.0f, 2.0f);

    for (size_t ch = 0; ch < nChannels; ++ch)
    {
        auto* data = block.getChannelPointer (ch);
        for (size_t i = 0; i < nSamples; ++i)
            data[i] = std::tanh (data[i] * tapeDrive) / tapeDrive;
    }
}

// =============================================================================
// CHERRY — Sweet Vocal Presence
// =============================================================================
void FlavorProcessor::processCherry (juce::dsp::AudioBlock<float>& block)
{
    float fizz = smoothedFizz.getCurrentValue();

    // Fizz-morphed parameters (shaped curves)
    float blend   = FizzCurves::sCurve (fizz, 0.10f, 0.65f);
    float drive   = FizzCurves::exponential (fizz, 1.5f, 4.5f, 2.5f);
    float presDb  = FizzCurves::logarithmic (fizz, 0.0f, 6.0f, 1.8f);
    float airDb   = FizzCurves::logarithmic (fizz, 0.0f, 4.0f, 1.8f);
    float harshDb = FizzCurves::logarithmic (fizz, 0.0f, -4.0f, 1.8f);

    // Oversampled parallel saturation via SaturationEngine (mix handles parallel blend)
    SaturationEngine::Params satParams;
    satParams.curve = SaturationEngine::CurveType::Tanh;
    satParams.drive = drive;
    satParams.outputGain = 1.0f / drive;  // Normalize: tanh(x*d)/d
    satParams.mix = blend;
    saturationEngine.process (block, satParams);

    // De-harsh notch @ 3.5kHz
    float harshGain = juce::Decibels::decibelsToGain (harshDb);
    *cherryDeHarsh.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter (
        sampleRate, 3500.0f, 2.0f, harshGain);
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        cherryDeHarsh.process (ctx);
    }

    // Presence bell @ 4.5kHz
    float presGain = juce::Decibels::decibelsToGain (presDb);
    *cherryPresence.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter (
        sampleRate, 4500.0f, 1.5f, presGain);
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        cherryPresence.process (ctx);
    }

    // Air shelf @ 12kHz
    float airGain = juce::Decibels::decibelsToGain (airDb);
    *cherryAirShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf (
        sampleRate, 12000.0f, 0.707f, airGain);
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        cherryAirShelf.process (ctx);
    }
}

void FlavorProcessor::processCherryFlat (juce::dsp::AudioBlock<float>& block)
{
    // FLAT: subtle chorus (1.5Hz rate, 0.5-1ms depth)
    const auto nChannels = block.getNumChannels();
    const auto nSamples = block.getNumSamples();
    float fizz = smoothedFizz.getCurrentValue();

    float chorusDepthMs = FizzCurves::sCurve (fizz, 0.5f, 1.0f);
    float chorusRate = 1.5f;
    float chorusMix = 0.3f;

    float depthSamples = static_cast<float>(chorusDepthMs * 0.001 * sampleRate);
    float baseDelaySamples = static_cast<float>(3.0 * 0.001 * sampleRate); // 3ms base
    float phaseInc = static_cast<float>(chorusRate * 2.0 * juce::MathConstants<double>::pi / sampleRate);
    int bufferSize = cherryChorusDelayBuffer.getNumSamples();

    for (size_t ch = 0; ch < nChannels; ++ch)
    {
        auto* data = block.getChannelPointer (ch);
        auto* delayData = cherryChorusDelayBuffer.getWritePointer (static_cast<int>(ch));

        for (size_t i = 0; i < nSamples; ++i)
        {
            int wp = (cherryChorusWritePos + static_cast<int>(i)) % bufferSize;
            delayData[wp] = data[i];

            float mod = std::sin (cherryChorusPhase + phaseInc * static_cast<float>(i)) * depthSamples;
            float readDelay = baseDelaySamples + mod;
            float readPos = static_cast<float>(wp) - readDelay;
            if (readPos < 0.0f) readPos += static_cast<float>(bufferSize);

            int readIdx = static_cast<int>(readPos);
            float frac = readPos - static_cast<float>(readIdx);
            int nextIdx = (readIdx + 1) % bufferSize;
            float delayed = delayData[readIdx] * (1.0f - frac) + delayData[nextIdx] * frac;

            data[i] = data[i] * (1.0f - chorusMix) + delayed * chorusMix;
        }
    }

    cherryChorusWritePos = (cherryChorusWritePos + static_cast<int>(nSamples)) % bufferSize;
    cherryChorusPhase += phaseInc * static_cast<float>(nSamples);
    if (cherryChorusPhase > juce::MathConstants<float>::twoPi)
        cherryChorusPhase -= juce::MathConstants<float>::twoPi;
}

// =============================================================================
// GRAPE — Lo-Fi Tape Texture
// =============================================================================
void FlavorProcessor::processGrape (juce::dsp::AudioBlock<float>& block)
{
    const auto nChannels = block.getNumChannels();
    const auto nSamples = block.getNumSamples();
    float fizz = smoothedFizz.getCurrentValue();

    // Fizz-morphed parameters (shaped curves)
    float tapeDrive   = FizzCurves::exponential (fizz, 1.2f, 4.0f, 2.5f);
    float wowDepthMs  = FizzCurves::exponential (fizz, 0.0f, 3.0f, 2.0f);
    float flutDepthMs = FizzCurves::exponential (fizz, 0.0f, 0.5f, 2.0f);
    float lpCutoff    = FizzCurves::logarithmic (fizz, 16000.0f, 4000.0f, 2.0f);

    // 1. Oversampled tape saturation with DC bias
    SaturationEngine::Params satParams;
    satParams.curve = SaturationEngine::CurveType::Tanh;
    satParams.drive = tapeDrive;
    satParams.dcBias = 0.15f;
    saturationEngine.process (block, satParams);

    // DC block
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        grapeDCBlocker.process (ctx);
    }

    // 2. Wow & Flutter via modulated delay
    float baseDelayMs = 5.0f;
    float baseDelaySamples = static_cast<float>(baseDelayMs * 0.001 * sampleRate);
    float wowDepthSamples = static_cast<float>(wowDepthMs * 0.001 * sampleRate);
    float flutDepthSamples = static_cast<float>(flutDepthMs * 0.001 * sampleRate);
    float wowPhaseInc = static_cast<float>(0.4 * 2.0 * juce::MathConstants<double>::pi / sampleRate);
    float flutPhaseInc = static_cast<float>(4.5 * 2.0 * juce::MathConstants<double>::pi / sampleRate);
    int delBufSize = grapeDelayBuffer.getNumSamples();

    for (size_t ch = 0; ch < nChannels; ++ch)
    {
        auto* data = block.getChannelPointer (ch);
        auto* delData = grapeDelayBuffer.getWritePointer (static_cast<int>(ch));

        for (size_t i = 0; i < nSamples; ++i)
        {
            int wp = (grapeDelayWritePos + static_cast<int>(i)) % delBufSize;
            delData[wp] = data[i];

            float wowMod = std::sin (grapeWowPhase + wowPhaseInc * static_cast<float>(i)) * wowDepthSamples;
            float flutPhase = grapeFlutterPhase + flutPhaseInc * static_cast<float>(i);
            float flutMod = (2.0f / juce::MathConstants<float>::pi) *
                            std::asin (std::sin (flutPhase)) * flutDepthSamples;

            float totalDelay = baseDelaySamples + wowMod + flutMod;
            totalDelay = juce::jlimit (1.0f, static_cast<float>(delBufSize - 2), totalDelay);

            float readPos = static_cast<float>(wp) - totalDelay;
            if (readPos < 0.0f) readPos += static_cast<float>(delBufSize);

            int readIdx = static_cast<int>(readPos);
            float frac = readPos - static_cast<float>(readIdx);
            int nextIdx = (readIdx + 1) % delBufSize;
            data[i] = delData[readIdx] * (1.0f - frac) + delData[nextIdx] * frac;
        }
    }

    grapeDelayWritePos = (grapeDelayWritePos + static_cast<int>(nSamples)) % delBufSize;
    grapeWowPhase += wowPhaseInc * static_cast<float>(nSamples);
    grapeFlutterPhase += flutPhaseInc * static_cast<float>(nSamples);
    if (grapeWowPhase > juce::MathConstants<float>::twoPi)
        grapeWowPhase -= juce::MathConstants<float>::twoPi;
    if (grapeFlutterPhase > juce::MathConstants<float>::twoPi)
        grapeFlutterPhase -= juce::MathConstants<float>::twoPi;

    // 3. Tape head LP filter
    grapeTapeLP.setCutoffFrequency (lpCutoff);
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        grapeTapeLP.process (ctx);
    }

}

void FlavorProcessor::processGrapeFlat (juce::dsp::AudioBlock<float>& block)
{
    // FLAT: Vinyl mode — crackle + rumble + mono below 300Hz
    processGrape (block);

    const auto nChannels = block.getNumChannels();
    const auto nSamples = block.getNumSamples();
    float fizz = smoothedFizz.getCurrentValue();

    // Vinyl crackle (sparse random pops)
    float crackleRate = FizzCurves::exponential (fizz, 0.001f, 0.01f, 2.0f);
    float crackleLevel = FizzCurves::exponential (fizz, 0.01f, 0.05f, 2.0f);
    for (size_t ch = 0; ch < nChannels; ++ch)
    {
        auto* data = block.getChannelPointer (ch);
        for (size_t i = 0; i < nSamples; ++i)
        {
            if (grapeNoiseRNG.nextFloat() < crackleRate)
                data[i] += (grapeNoiseRNG.nextFloat() * 2.0f - 1.0f) * crackleLevel;
        }
    }

    // 40Hz rumble
    float rumbleLevel = FizzCurves::exponential (fizz, 0.0f, 0.015f, 2.0f);
    for (size_t ch = 0; ch < nChannels; ++ch)
    {
        auto* data = block.getChannelPointer (ch);
        for (size_t i = 0; i < nSamples; ++i)
        {
            float noise = (grapeNoiseRNG.nextFloat() * 2.0f - 1.0f) * rumbleLevel;
            data[i] += noise;
        }
    }

    // Mono below 300Hz (stereo only)
    if (nChannels >= 2)
    {
        auto* dataL = block.getChannelPointer (0);
        auto* dataR = block.getChannelPointer (1);
        for (size_t i = 0; i < nSamples; ++i)
        {
            float mid = (dataL[i] + dataR[i]) * 0.5f;
            float sideL = dataL[i] - mid;
            float sideR = dataR[i] - mid;
            dataL[i] = mid + sideL;
            dataR[i] = mid + sideR;
        }
    }
}

// =============================================================================
// LEMON-LIME — Crisp Exciter
// =============================================================================
void FlavorProcessor::processLemonLime (juce::dsp::AudioBlock<float>& block)
{
    const auto nChannels = block.getNumChannels();
    const auto nSamples = block.getNumSamples();
    float fizz = smoothedFizz.getCurrentValue();

    // Fizz-morphed parameters (shaped curves)
    float crossoverFreq = FizzCurves::logarithmic (fizz, 4000.0f, 1500.0f, 2.0f);
    float hfDrive       = FizzCurves::exponential (fizz, 1.3f, 3.5f, 2.5f);
    float presDb        = FizzCurves::logarithmic (fizz, 0.5f, 6.0f, 1.8f);
    float airDb         = FizzCurves::logarithmic (fizz, 0.5f, 5.0f, 1.8f);
    float compAttack    = FizzCurves::exponential (fizz, 10.0f, 0.5f, 2.0f);

    // 1. True Linkwitz-Riley 4th-order crossover (cascaded 2nd-order)
    lemonLowBandBuffer.setSize (static_cast<int>(nChannels), static_cast<int>(nSamples), false, false, true);
    for (size_t ch = 0; ch < nChannels; ++ch)
        lemonLowBandBuffer.copyFrom (static_cast<int>(ch), 0,
                                     block.getChannelPointer (ch),
                                     static_cast<int>(nSamples));

    // Low-pass through two cascaded stages (LR4)
    lemonLowPass1.setCutoffFrequency (crossoverFreq);
    lemonLowPass2.setCutoffFrequency (crossoverFreq);
    {
        juce::dsp::AudioBlock<float> lowBlock (lemonLowBandBuffer);
        juce::dsp::ProcessContextReplacing<float> ctx (lowBlock);
        lemonLowPass1.process (ctx);
    }
    {
        juce::dsp::AudioBlock<float> lowBlock (lemonLowBandBuffer);
        juce::dsp::ProcessContextReplacing<float> ctx (lowBlock);
        lemonLowPass2.process (ctx);
    }

    // High-pass through two cascaded stages (LR4)
    lemonHighPass1.setCutoffFrequency (crossoverFreq);
    lemonHighPass2.setCutoffFrequency (crossoverFreq);
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        lemonHighPass1.process (ctx);
    }
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        lemonHighPass2.process (ctx);
    }

    // 2. Oversampled HF band saturation via SaturationEngine
    SaturationEngine::Params satParams;
    satParams.curve = SaturationEngine::CurveType::Tanh;
    satParams.drive = hfDrive;
    satParams.outputGain = 1.0f / hfDrive;
    saturationEngine.process (block, satParams);

    // 3. Fast envelope compressor on HF
    lemonHFCompressor.setRatio (4.0f);
    lemonHFCompressor.setThreshold (-20.0f);
    lemonHFCompressor.setAttack (compAttack);
    lemonHFCompressor.setRelease (50.0f);
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        lemonHFCompressor.process (ctx);
    }

    // 4. Presence bell @ 5kHz
    float presGain = juce::Decibels::decibelsToGain (presDb);
    *lemonPresence.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter (
        sampleRate, 5000.0f, 1.5f, presGain);
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        lemonPresence.process (ctx);
    }

    // 5. Air shelf @ 10kHz
    float airGain = juce::Decibels::decibelsToGain (airDb);
    *lemonAirShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf (
        sampleRate, 10000.0f, 0.707f, airGain);
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        lemonAirShelf.process (ctx);
    }

    // 6. Sum LOW + HIGH (LR4 sums flat)
    for (size_t ch = 0; ch < nChannels; ++ch)
    {
        auto* outData = block.getChannelPointer (ch);
        const auto* lowData = lemonLowBandBuffer.getReadPointer (static_cast<int>(ch));
        for (size_t i = 0; i < nSamples; ++i)
            outData[i] += lowData[i];
    }
}

void FlavorProcessor::processLemonLimeFlat (juce::dsp::AudioBlock<float>& block)
{
    // FLAT: Telephone EQ — bandpass 300Hz–3.5kHz with resonant peaks
    float fizz = smoothedFizz.getCurrentValue();
    float resonance = FizzCurves::exponential (fizz, 0.707f, 3.0f, 2.0f);

    *lemonTeleBandpass.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass (
        sampleRate, 300.0f, resonance);
    *lemonTeleHighCut.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass (
        sampleRate, 3500.0f, resonance);

    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        lemonTeleBandpass.process (ctx);
    }
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        lemonTeleHighCut.process (ctx);
    }
}

// =============================================================================
// ORANGE CREAM — Stereo Width + Warmth
// =============================================================================
void FlavorProcessor::processOrangeCream (juce::dsp::AudioBlock<float>& block)
{
    const auto nChannels = block.getNumChannels();
    const auto nSamples = block.getNumSamples();
    float fizz = smoothedFizz.getCurrentValue();

    // Fizz-morphed parameters (shaped curves)
    float warmDrive    = FizzCurves::exponential (fizz, 1.2f, 2.5f, 2.0f);
    float sideGainDb   = FizzCurves::sCurve (fizz, 0.0f, 5.0f);
    float detuneCents  = FizzCurves::sCurve (fizz, 3.0f, 10.0f);
    float midCompRatio = FizzCurves::exponential (fizz, 1.5f, 4.0f, 2.0f);
    float hiShelfDb    = FizzCurves::logarithmic (fizz, 0.0f, -3.0f, 1.8f);

    // 1. Oversampled gentle warm saturation
    SaturationEngine::Params satParams;
    satParams.curve = SaturationEngine::CurveType::WarmClip;
    satParams.drive = warmDrive;
    saturationEngine.process (block, satParams);

    // For stereo processing, need at least 2 channels
    if (nChannels >= 2)
    {
        auto* dataL = block.getChannelPointer (0);
        auto* dataR = block.getChannelPointer (1);

        // 2. Mid-Side encode
        for (size_t i = 0; i < nSamples; ++i)
        {
            float mid  = (dataL[i] + dataR[i]) * 0.5f;
            float side = (dataL[i] - dataR[i]) * 0.5f;
            dataL[i] = mid;
            dataR[i] = side;
        }

        // 3. MID compression (fix: actually process through compressor)
        orangeMidCompressor.setRatio (midCompRatio);
        orangeMidCompressor.setThreshold (-18.0f);
        orangeMidCompressor.setAttack (15.0f);
        orangeMidCompressor.setRelease (100.0f);
        {
            // Process mid channel (stored in L) through compressor
            // Create a single-channel block wrapping just the L channel
            float* midChannelPtr = dataL;
            juce::dsp::AudioBlock<float> midBlock (&midChannelPtr, 1, nSamples);
            juce::dsp::ProcessContextReplacing<float> ctx (midBlock);
            orangeMidCompressor.process (ctx);
        }

        // 4. SIDE gain boost for width
        float sideGain = juce::Decibels::decibelsToGain (sideGainDb);
        for (size_t i = 0; i < nSamples; ++i)
            dataR[i] *= sideGain;

        // 5. Mid-Side decode back to L/R
        for (size_t i = 0; i < nSamples; ++i)
        {
            float mid  = dataL[i];
            float side = dataR[i];
            dataL[i] = mid + side;
            dataR[i] = mid - side;
        }

        // 6. Micro-Pitch Detune
        float detuneRatio = std::pow (2.0f, detuneCents / 1200.0f);
        float detuneRatioDown = std::pow (2.0f, -detuneCents / 1200.0f);

        int delBufSize = orangeDetuneDelayL.getNumSamples();
        auto* delL = orangeDetuneDelayL.getWritePointer (0);
        auto* delR = orangeDetuneDelayR.getWritePointer (0);

        for (size_t i = 0; i < nSamples; ++i)
        {
            delL[orangeDetuneWritePosL] = dataL[i];
            delR[orangeDetuneWritePosR] = dataR[i];

            // Left channel: pitch up (read faster)
            orangeDetunePitchPhaseL += detuneRatio;
            while (orangeDetunePitchPhaseL >= static_cast<float>(delBufSize))
                orangeDetunePitchPhaseL -= static_cast<float>(delBufSize);

            float readPosL = static_cast<float>(orangeDetuneWritePosL) - orangeDetunePitchPhaseL;
            if (readPosL < 0.0f) readPosL += static_cast<float>(delBufSize);
            int rIdxL = static_cast<int>(readPosL) % delBufSize;
            int rNextL = (rIdxL + 1) % delBufSize;
            float fracL = readPosL - std::floor (readPosL);
            float detunedL = delL[rIdxL] * (1.0f - fracL) + delL[rNextL] * fracL;

            // Right channel: pitch down (read slower)
            orangeDetunePitchPhaseR += detuneRatioDown;
            while (orangeDetunePitchPhaseR >= static_cast<float>(delBufSize))
                orangeDetunePitchPhaseR -= static_cast<float>(delBufSize);

            float readPosR = static_cast<float>(orangeDetuneWritePosR) - orangeDetunePitchPhaseR;
            if (readPosR < 0.0f) readPosR += static_cast<float>(delBufSize);
            int rIdxR = static_cast<int>(readPosR) % delBufSize;
            int rNextR = (rIdxR + 1) % delBufSize;
            float fracR = readPosR - std::floor (readPosR);
            float detunedR = delR[rIdxR] * (1.0f - fracR) + delR[rNextR] * fracR;

            dataL[i] = dataL[i] * 0.5f + detunedL * 0.5f;
            dataR[i] = dataR[i] * 0.5f + detunedR * 0.5f;

            orangeDetuneWritePosL = (orangeDetuneWritePosL + 1) % delBufSize;
            orangeDetuneWritePosR = (orangeDetuneWritePosR + 1) % delBufSize;
        }
    }

    // 7. Warm high shelf rolloff @ 10kHz
    float shelfGain = juce::Decibels::decibelsToGain (hiShelfDb);
    *orangeWarmShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf (
        sampleRate, 10000.0f, 0.707f, shelfGain);
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        orangeWarmShelf.process (ctx);
    }
}

void FlavorProcessor::processOrangeCreamFlat (juce::dsp::AudioBlock<float>& block)
{
    // FLAT: simplified — warm sat → M/S → side gain → M/S decode → warm shelf
    const auto nChannels = block.getNumChannels();
    const auto nSamples = block.getNumSamples();
    float fizz = smoothedFizz.getCurrentValue();

    // Fizz-morphed parameters (shaped curves)
    float warmDrive    = FizzCurves::exponential (fizz, 1.2f, 2.5f, 2.0f);
    float sideGainDb   = FizzCurves::sCurve (fizz, 0.0f, 5.0f);
    float hiShelfDb    = FizzCurves::logarithmic (fizz, 0.0f, -3.0f, 1.8f);

    // 1. Oversampled warm saturation
    SaturationEngine::Params satParams;
    satParams.curve = SaturationEngine::CurveType::WarmClip;
    satParams.drive = warmDrive;
    saturationEngine.process (block, satParams);

    if (nChannels >= 2)
    {
        auto* dataL = block.getChannelPointer (0);
        auto* dataR = block.getChannelPointer (1);

        // Mid-Side encode
        for (size_t i = 0; i < nSamples; ++i)
        {
            float mid  = (dataL[i] + dataR[i]) * 0.5f;
            float side = (dataL[i] - dataR[i]) * 0.5f;
            dataL[i] = mid;
            dataR[i] = side;
        }

        // Harmonic enhancer on mids (inline — avoids double-pumping the oversampler
        // and channel-count mismatch with the 2-channel oversampling object)
        for (size_t i = 0; i < nSamples; ++i)
            dataL[i] = std::tanh (dataL[i] * 2.0f) * 0.5f;

        // SIDE: gain boost (no Haas delay — mono collapse risk removed)
        float sideGain = juce::Decibels::decibelsToGain (sideGainDb);
        for (size_t i = 0; i < nSamples; ++i)
            dataR[i] *= sideGain;

        // Mid-Side decode
        for (size_t i = 0; i < nSamples; ++i)
        {
            float mid  = dataL[i];
            float side = dataR[i];
            dataL[i] = mid + side;
            dataR[i] = mid - side;
        }
    }

    // Warm high shelf rolloff
    float shelfGain = juce::Decibels::decibelsToGain (hiShelfDb);
    *orangeWarmShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf (
        sampleRate, 10000.0f, 0.707f, shelfGain);
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        orangeWarmShelf.process (ctx);
    }
}
