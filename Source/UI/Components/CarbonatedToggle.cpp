#include "CarbonatedToggle.h"
#include "Parameters/ParameterIDs.h"
#include "UI/LookAndFeel/ColorScheme.h"

CarbonatedToggle::CarbonatedToggle (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts)
{
    // Setup status label above toggle (LARGER for readability)
    statusLabel.setText ("CARBONATED", juce::dontSendNotification);
    statusLabel.setFont (juce::Font (13.0f, juce::Font::bold));
    statusLabel.setJustificationType (juce::Justification::centred);
    statusLabel.setColour (juce::Label::textColourId, SodaColors::Theme::getTextHighlight());
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

    // Poll for flavor color changes at 15Hz
    startTimerHz (15);
}

CarbonatedToggle::~CarbonatedToggle()
{
    stopTimer();
}

void CarbonatedToggle::timerCallback()
{
    updateLabel();
}

void CarbonatedToggle::updateLabel()
{
    if (carbonatedButton.getToggleState())
    {
        statusLabel.setText ("CARBONATED", juce::dontSendNotification);
        statusLabel.setColour (juce::Label::textColourId, SodaColors::Theme::getTextHighlight());
    }
    else
    {
        statusLabel.setText ("FLAT", juce::dontSendNotification);
        statusLabel.setColour (juce::Label::textColourId, SodaColors::Theme::getTextSecondary());
    }
}

void CarbonatedToggle::paint (juce::Graphics& g)
{
    juce::ignoreUnused (g);
}

void CarbonatedToggle::resized()
{
    auto bounds = getLocalBounds();

    // Label on top (full width for "CARBONATED" text)
    statusLabel.setBounds (bounds.removeFromTop (22));

    bounds.removeFromTop (6);  // Gap

    // Toggle button below (centered, narrow vertical rocker)
    auto toggleWidth = 48;  // Vertical rocker stays narrow
    auto toggleButton = bounds.withSizeKeepingCentre (toggleWidth, bounds.getHeight());
    carbonatedButton.setBounds (toggleButton);
}
