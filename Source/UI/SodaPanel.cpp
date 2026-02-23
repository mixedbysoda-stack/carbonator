#include "SodaPanel.h"
#include "LookAndFeel/ColorScheme.h"

SodaPanel::SodaPanel (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts),
      fizzKnob (apvts),
      flavorSelector (apvts),
      carbonatedToggle (apvts)
{
    addAndMakeVisible (fizzKnob);
    addAndMakeVisible (flavorSelector);
    addAndMakeVisible (carbonatedToggle);
}

void SodaPanel::paint (juce::Graphics& g)
{
    juce::ignoreUnused (g);
}

void SodaPanel::resized()
{
    auto bounds = getLocalBounds().reduced (20);

    // FIZZ knob in center (takes most space)
    auto fizzBounds = bounds.removeFromTop (static_cast<int>(bounds.getHeight() * 0.7f));
    fizzKnob.setBounds (fizzBounds);

    bounds.removeFromTop (20);  // Spacing

    // Footer section with flavor and carbonated toggle
    auto footerHeight = 60;
    auto footerBounds = bounds.removeFromBottom (footerHeight);

    // Split footer in half
    auto flavorBounds = footerBounds.removeFromLeft (footerBounds.getWidth() / 2).reduced (10, 0);
    flavorSelector.setBounds (flavorBounds);

    auto toggleBounds = footerBounds.reduced (10, 0);
    carbonatedToggle.setBounds (toggleBounds);
}
