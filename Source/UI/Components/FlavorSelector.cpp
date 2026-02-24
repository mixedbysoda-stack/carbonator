#include "FlavorSelector.h"
#include "Parameters/ParameterIDs.h"
#include "UI/LookAndFeel/ColorScheme.h"

FlavorSelector::FlavorSelector (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts)
{
    // Setup title label
    titleLabel.setText ("FLAVOR", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (13.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel);

    // Setup large flavor name label
    flavorLabel.setText ("COLA", juce::dontSendNotification);
    flavorLabel.setFont (juce::Font (28.0f, juce::Font::bold));
    flavorLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (flavorLabel);

    // Setup combo box (hidden, but functional for parameter control)
    flavorCombo.addItem ("COLA", 1);
    flavorCombo.addItem ("CHERRY", 2);
    flavorCombo.addItem ("GRAPE", 3);
    flavorCombo.addItem ("LEMON-LIME", 4);
    flavorCombo.addItem ("ORANGE CREAM", 5);
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
        case 1: flavorLabel.setText ("COLA", juce::dontSendNotification); break;
        case 2: flavorLabel.setText ("CHERRY", juce::dontSendNotification); break;
        case 3: flavorLabel.setText ("GRAPE", juce::dontSendNotification); break;
        case 4: flavorLabel.setText ("LEMON-LIME", juce::dontSendNotification); break;
        case 5: flavorLabel.setText ("ORANGE CREAM", juce::dontSendNotification); break;
        default: flavorLabel.setText ("COLA", juce::dontSendNotification); break;
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
    titleLabel.setBounds (bounds.removeFromTop (20));

    bounds.removeFromTop (4);

    // Large flavor name display
    flavorLabel.setBounds (bounds.removeFromTop (36));

    // ComboBox takes same space
    flavorCombo.setBounds (flavorLabel.getBounds());
}
