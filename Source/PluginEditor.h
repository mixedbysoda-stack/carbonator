#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/LookAndFeel/SodaLookAndFeel.h"
#include "UI/SodaPanel.h"

//==============================================================================
/**
 * Soda Filter Editor
 * Retro soda can aesthetic with vertical layout
 */
class SodaFilterAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SodaFilterAudioProcessorEditor (SodaFilterAudioProcessor&);
    ~SodaFilterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Reference to processor
    SodaFilterAudioProcessor& audioProcessor;

    // Custom look and feel
    SodaLookAndFeel sodaLAF;

    // Main panel
    SodaPanel sodaPanel;

    // Title labels
    juce::Label titleLabel;
    juce::Label subtitleLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SodaFilterAudioProcessorEditor)
};
