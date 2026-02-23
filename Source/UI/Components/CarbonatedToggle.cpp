#include "CarbonatedToggle.h"
#include "Parameters/ParameterIDs.h"
#include "UI/LookAndFeel/ColorScheme.h"

CarbonatedToggle::CarbonatedToggle (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts)
{
    // Setup status label above toggle
    statusLabel.setText ("CARBONATED", juce::dontSendNotification);
    statusLabel.setFont (juce::Font (10.0f, juce::Font::bold));
    statusLabel.setJustificationType (juce::Justification::centred);
    statusLabel.setColour (juce::Label::textColourId, SodaColors::sodaRed);
    addAndMakeVisible (statusLabel);

    // Setup toggle button (no text, just visual)
    carbonatedButton.setButtonText ("");
    addAndMakeVisible (carbonatedButton);

    // Attach to parameter
    using namespace ParameterIDs::Filter;
    carbonatedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        apvts, carbonated.getParamID(), carbonatedButton
    );

    // Update label when toggle changes
    carbonatedButton.onClick = [this]() { updateLabel(); };

    updateLabel();
}

void CarbonatedToggle::updateLabel()
{
    if (carbonatedButton.getToggleState())
    {
        statusLabel.setText ("CARBONATED", juce::dontSendNotification);
        statusLabel.setColour (juce::Label::textColourId, SodaColors::sodaRed);
    }
    else
    {
        statusLabel.setText ("FLAT", juce::dontSendNotification);
        statusLabel.setColour (juce::Label::textColourId, SodaColors::textSecondary);
    }
}

void CarbonatedToggle::paint (juce::Graphics& g)
{
    juce::ignoreUnused (g);
}

void CarbonatedToggle::resized()
{
    auto bounds = getLocalBounds();

    // Label on top
    statusLabel.setBounds (bounds.removeFromTop (16));

    bounds.removeFromTop (4);  // Gap

    // Toggle button below
    carbonatedButton.setBounds (bounds);
}
