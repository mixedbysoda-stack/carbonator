#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "UI/LookAndFeel/ColorScheme.h"

//==============================================================================
SodaFilterAudioProcessorEditor::SodaFilterAudioProcessorEditor (SodaFilterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), sodaPanel (p.getAPVTS())
{
    // Set look and feel
    setLookAndFeel (&sodaLAF);

    // Setup title label
    titleLabel.setText ("SODA FILTER", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (32.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, SodaColors::textHighlight);
    addAndMakeVisible (titleLabel);

    // Setup subtitle label
    subtitleLabel.setText ("VINTAGE CARBONATED AUDIO", juce::dontSendNotification);
    subtitleLabel.setFont (juce::Font (14.0f));
    subtitleLabel.setJustificationType (juce::Justification::centred);
    subtitleLabel.setColour (juce::Label::textColourId, SodaColors::textSecondary);
    addAndMakeVisible (subtitleLabel);

    // Add main panel
    addAndMakeVisible (sodaPanel);

    // Set default size (420 x 560, vertical like a soda can)
    setSize (420, 560);
    setResizable (true, true);
    setResizeLimits (350, 450, 600, 800);
}

SodaFilterAudioProcessorEditor::~SodaFilterAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void SodaFilterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Cream background
    g.fillAll (SodaColors::background);

    // Red border (rounded rectangle)
    auto bounds = getLocalBounds().toFloat().reduced (10.0f);
    g.setColour (SodaColors::border);
    g.drawRoundedRectangle (bounds, 32.0f, 8.0f);
}

void SodaFilterAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (20);

    // Title at top
    titleLabel.setBounds (bounds.removeFromTop (40));

    // Subtitle
    subtitleLabel.setBounds (bounds.removeFromTop (25));

    bounds.removeFromTop (10);  // Spacing

    // Main panel takes remaining space
    sodaPanel.setBounds (bounds);
}
