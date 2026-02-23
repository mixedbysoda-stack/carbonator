#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "DSP/EffectsChain.h"
#include "Parameters/ParameterIDs.h"

//==============================================================================
/**
 * Soda Filter Audio Processor
 *
 * Underwater/soda can effect with flavor variations
 * Signal flow: Input → Low-Pass Filter → Flavor Effect → Output
 */
class SodaFilterAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    SodaFilterAudioProcessor();
    ~SodaFilterAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Expose APVTS for UI binding
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Undo/Redo functionality
    juce::UndoManager& getUndoManager() { return undoManager; }

private:
    //==============================================================================
    // Parameter management
    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // DSP processing chain
    std::unique_ptr<EffectsChain> effectsChain;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SodaFilterAudioProcessor)
};
