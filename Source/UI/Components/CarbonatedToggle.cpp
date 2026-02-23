#include "CarbonatedToggle.h"
#include "Parameters/ParameterIDs.h"
#include "UI/LookAndFeel/ColorScheme.h"

CarbonatedToggle::CarbonatedToggle (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts)
{
    // Setup toggle button
    carbonatedButton.setButtonText ("CARBONATED");
    addAndMakeVisible (carbonatedButton);

    // Attach to parameter
    using namespace ParameterIDs::Filter;
    carbonatedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        apvts, carbonated.getParamID(), carbonatedButton
    );
}

void CarbonatedToggle::paint (juce::Graphics& g)
{
    juce::ignoreUnused (g);
}

void CarbonatedToggle::resized()
{
    carbonatedButton.setBounds (getLocalBounds());
}
