#include "FlavorSelector.h"
#include "Parameters/ParameterIDs.h"
#include "UI/LookAndFeel/ColorScheme.h"

FlavorSelector::FlavorSelector (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts)
{
    // Setup title label
    titleLabel.setText ("FLAVOR", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (12.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    titleLabel.setColour (juce::Label::textColourId, SodaColors::textSecondary);
    addAndMakeVisible (titleLabel);

    // Setup combo box
    flavorCombo.addItem ("CHERRY", 1);
    flavorCombo.addItem ("GRAPE", 2);
    flavorCombo.addItem ("DIRTY SODA", 3);
    addAndMakeVisible (flavorCombo);

    // Attach to parameter
    using namespace ParameterIDs::Flavor;
    flavorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        apvts, type.getParamID(), flavorCombo
    );
}

void FlavorSelector::paint (juce::Graphics& g)
{
    juce::ignoreUnused (g);
}

void FlavorSelector::resized()
{
    auto bounds = getLocalBounds();
    titleLabel.setBounds (bounds.removeFromTop (20));
    flavorCombo.setBounds (bounds.reduced (0, 2));
}
