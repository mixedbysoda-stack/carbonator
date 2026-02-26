#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * Carbonated toggle component
 * ON = fizzy, OFF = flat (extra low-pass)
 */
class CarbonatedToggle : public juce::Component,
                         private juce::Timer
{
public:
    CarbonatedToggle (juce::AudioProcessorValueTreeState& apvts);
    ~CarbonatedToggle() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    juce::Label statusLabel;  // Shows "CARBONATED" or "FLAT"
    juce::ToggleButton carbonatedButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> carbonatedAttachment;

    void updateLabel();
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CarbonatedToggle)
};
