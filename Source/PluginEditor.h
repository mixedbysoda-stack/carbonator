#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/LookAndFeel/SodaLookAndFeel.h"
#include "UI/SodaPanel.h"

#ifndef CARBONATOR_DEMO
 #include "Licensing/ActivationDialog.h"
#endif

//==============================================================================
/**
 * Soda Filter Editor
 * Retro soda can aesthetic with vertical layout
 */
class SodaFilterAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    SodaFilterAudioProcessorEditor (SodaFilterAudioProcessor&);
    ~SodaFilterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void parentHierarchyChanged() override;

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

    // Custom fonts
    juce::Font titleFont;
    juce::Font subtitleFont;

    // Theme toggle button
    juce::TextButton themeToggleButton;
    void toggleTheme();

    // Bubble animation
    struct Bubble
    {
        float x;           // Position (0-1 normalized)
        float y;           // Position (0-1 normalized)
        float size;        // Diameter in pixels
        float speed;       // Rise speed
        float delay;       // Animation delay
        float lifetime;    // Time alive
    };

    std::vector<Bubble> bubbles;
    void generateBubbles();
    void updateBubbles();

#ifndef CARBONATOR_DEMO
    std::unique_ptr<ActivationDialog> activationDialog;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SodaFilterAudioProcessorEditor)
};
