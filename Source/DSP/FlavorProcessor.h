#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "Parameters/ParameterIDs.h"
#include "SaturationEngine.h"

/**
 * Flavor effect processor v2.0
 * 5 flavors, each with distinct DSP chain and multi-parameter Fizz morphing.
 * Carbonated toggle provides per-flavor alternate mode.
 */
class FlavorProcessor
{
public:
    FlavorProcessor (juce::AudioProcessorValueTreeState& apvts);

    void prepare (const juce::dsp::ProcessSpec& spec);
    void process (juce::dsp::ProcessContextReplacing<float>& context);
    void reset();

    /** Set the current Fizz amount (0-1) — called from EffectsChain each block */
    void setFizzAmount (float newFizz);

    /** Set the Carbonated state — called from EffectsChain each block */
    void setCarbonated (bool isCarbonated);

    /** Toggle oversampling quality mode */
    void setQualityMode (bool hq);

    /** Get oversampling latency in samples */
    float getLatencyInSamples() const;

private:
    // Per-flavor process methods
    void processCola (juce::dsp::AudioBlock<float>& block);
    void processCherry (juce::dsp::AudioBlock<float>& block);
    void processGrape (juce::dsp::AudioBlock<float>& block);
    void processLemonLime (juce::dsp::AudioBlock<float>& block);
    void processOrangeCream (juce::dsp::AudioBlock<float>& block);

    // Per-flavor FLAT alternate modes
    void processColaFlat (juce::dsp::AudioBlock<float>& block);
    void processCherryFlat (juce::dsp::AudioBlock<float>& block);
    void processGrapeFlat (juce::dsp::AudioBlock<float>& block);
    void processLemonLimeFlat (juce::dsp::AudioBlock<float>& block);
    void processOrangeCreamFlat (juce::dsp::AudioBlock<float>& block);

    // ─── Shared saturation engine (oversampled) ─────────────────
    SaturationEngine saturationEngine;

    // ─── Shared state ───────────────────────────────────────────
    juce::AudioParameterChoice* flavorTypeParam;
    juce::SmoothedValue<float> smoothedFizz;
    bool carbonatedState = true;
    double sampleRate = 44100.0;
    int numChannels = 2;
    int blockSize = 512;

    // ─── COLA DSP members ───────────────────────────────────────
    juce::dsp::Compressor<float> colaCompressor;
    juce::dsp::StateVariableTPTFilter<float> colaDCBlocker;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> colaLowShelf;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> colaHighShelf;

    // ─── CHERRY DSP members ─────────────────────────────────────
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> cherryDeHarsh;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> cherryPresence;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> cherryAirShelf;
    // FLAT: chorus
    float cherryChorusPhase = 0.0f;
    juce::AudioBuffer<float> cherryChorusDelayBuffer;
    int cherryChorusWritePos = 0;

    // ─── GRAPE DSP members ──────────────────────────────────────
    juce::dsp::StateVariableTPTFilter<float> grapeDCBlocker;
    juce::dsp::StateVariableTPTFilter<float> grapeTapeLP;
    juce::AudioBuffer<float> grapeDelayBuffer;
    int grapeDelayWritePos = 0;
    float grapeWowPhase = 0.0f;
    float grapeFlutterPhase = 0.0f;
    juce::Random grapeNoiseRNG;
    // FLAT: vinyl rumble
    juce::dsp::StateVariableTPTFilter<float> grapeVinylRumbleLP;

    // ─── LEMON-LIME DSP members ─────────────────────────────────
    // True Linkwitz-Riley 4th-order (cascaded 2nd-order SVTPF)
    juce::dsp::StateVariableTPTFilter<float> lemonLowPass1;
    juce::dsp::StateVariableTPTFilter<float> lemonLowPass2;
    juce::dsp::StateVariableTPTFilter<float> lemonHighPass1;
    juce::dsp::StateVariableTPTFilter<float> lemonHighPass2;
    juce::AudioBuffer<float> lemonLowBandBuffer;
    juce::dsp::Compressor<float> lemonHFCompressor;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> lemonPresence;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> lemonAirShelf;
    // FLAT: telephone EQ
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> lemonTeleBandpass;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> lemonTeleHighCut;

    // ─── ORANGE CREAM DSP members (Lowpass Filter + Drive) ──────
    juce::dsp::StateVariableTPTFilter<float> orangeLP1;   // Resonant LPF stage 1
    juce::dsp::StateVariableTPTFilter<float> orangeLP2;   // Steep LPF stage 2
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> orangeLowShelf;
};
