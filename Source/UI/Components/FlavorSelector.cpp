#include "FlavorSelector.h"
#include "Parameters/ParameterIDs.h"
#include "UI/LookAndFeel/ColorScheme.h"

FlavorSelector::FlavorSelector (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts)
{
    // Setup title label (small, uppercase) - will update color on paint
    titleLabel.setText ("FLAVOR", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (10.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel);

    // Setup large flavor name label - will update color on paint
    flavorLabel.setText ("CHERRY", juce::dontSendNotification);
    flavorLabel.setFont (juce::Font (20.0f, juce::Font::bold));
    flavorLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (flavorLabel);

    // Setup combo box (hidden, but functional for parameter control)
    flavorCombo.addItem ("CHERRY", 1);
    flavorCombo.addItem ("GRAPE", 2);
    flavorCombo.addItem ("DIRTY SODA", 3);
    addAndMakeVisible (flavorCombo);

    // Attach to parameter
    using namespace ParameterIDs::Flavor;
    flavorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        apvts, type.getParamID(), flavorCombo
    );

    // Update label when combo changes
    flavorCombo.onChange = [this]() { updateFlavorLabel(); };

    updateFlavorLabel();
}

void FlavorSelector::updateFlavorLabel()
{
    int selectedId = flavorCombo.getSelectedId();
    switch (selectedId)
    {
        case 1: flavorLabel.setText ("CHERRY", juce::dontSendNotification); break;
        case 2: flavorLabel.setText ("GRAPE", juce::dontSendNotification); break;
        case 3: flavorLabel.setText ("DIRTY SODA", juce::dontSendNotification); break;
        default: flavorLabel.setText ("CHERRY", juce::dontSendNotification); break;
    }
}

void FlavorSelector::paint (juce::Graphics& g)
{
    juce::ignoreUnused (g);

    // Update label colors based on theme
    titleLabel.setColour (juce::Label::textColourId,
        SodaColors::Theme::getTextHighlight().withAlpha (0.6f));
    flavorLabel.setColour (juce::Label::textColourId,
        SodaColors::Theme::getTextHighlight());
}

void FlavorSelector::resized()
{
    auto bounds = getLocalBounds();

    // Small label at top
    titleLabel.setBounds (bounds.removeFromTop (16));

    bounds.removeFromTop (2);  // Gap

    // Large flavor name display
    flavorLabel.setBounds (bounds.removeFromTop (28));

    // ComboBox takes same space (will be styled to look like the label)
    flavorCombo.setBounds (flavorLabel.getBounds());
}
