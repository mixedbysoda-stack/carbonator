#include "FizzKnob.h"
#include "Parameters/ParameterIDs.h"
#include "UI/LookAndFeel/ColorScheme.h"

FizzKnob::FizzKnob (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts)
{
    // Setup FIZZ slider
    fizzSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    fizzSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (fizzSlider);

    // Attach to parameter
    using namespace ParameterIDs::Filter;
    fizzAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, fizzAmount.getParamID(), fizzSlider
    );

    // Setup percentage label
    percentageLabel.setFont (juce::Font (24.0f, juce::Font::bold));
    percentageLabel.setJustificationType (juce::Justification::centred);
    percentageLabel.setColour (juce::Label::textColourId, SodaColors::textPrimary);
    addAndMakeVisible (percentageLabel);

    // Setup title label
    titleLabel.setText ("FIZZ", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, SodaColors::textSecondary);
    addAndMakeVisible (titleLabel);

    // Update percentage on value change
    fizzSlider.onValueChange = [this]()
    {
        int percent = static_cast<int>(fizzSlider.getValue());
        percentageLabel.setText (juce::String (percent) + "%", juce::dontSendNotification);
    };

    // Initialize percentage
    fizzSlider.onValueChange();
}

void FizzKnob::paint (juce::Graphics& g)
{
    juce::ignoreUnused (g);
}

void FizzKnob::resized()
{
    auto bounds = getLocalBounds();

    // Title at top
    titleLabel.setBounds (bounds.removeFromTop (30));

    // Large knob in center
    auto knobSize = juce::jmin (bounds.getWidth(), bounds.getHeight() - 40);
    auto knobBounds = bounds.removeFromTop (knobSize).withSizeKeepingCentre (knobSize, knobSize);
    fizzSlider.setBounds (knobBounds);

    // Percentage below knob
    percentageLabel.setBounds (bounds.removeFromTop (40));
}
