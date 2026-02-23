#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * Large FIZZ knob component
 * Main control for underwater → brightness transition
 */
class FizzKnob : public juce::Component,
                 private juce::Timer
{
public:
    FizzKnob (juce::AudioProcessorValueTreeState& apvts);

    void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    juce::Slider fizzSlider;
    juce::Label percentageLabel;
    juce::Label titleLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fizzAttachment;

    // Pulsing animation
    float pulsePhase = 0.0f;
    bool isInteracting = false;
    float lastValue = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FizzKnob)
};
