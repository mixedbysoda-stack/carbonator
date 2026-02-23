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
    auto bounds = getLocalBounds();

    // Footer section (theme-aware background)
    auto footerBounds = bounds.removeFromBottom (90);
    g.setColour (SodaColors::Theme::getFooterBackground());
    g.fillRect (footerBounds);

    // Top border for footer
    g.setColour (SodaColors::Theme::getBorder().withAlpha (0.2f));
    g.drawHorizontalLine (footerBounds.getY(), static_cast<float>(footerBounds.getX()),
                          static_cast<float>(footerBounds.getRight()));
}

void SodaPanel::resized()
{
    auto bounds = getLocalBounds();

    // Footer section (bigger to fit larger text - 110px)
    auto footerBounds = bounds.removeFromBottom (110);
    auto footerContent = footerBounds.reduced (20, 15);

    // Flavor on left
    auto flavorBounds = footerContent.removeFromLeft (footerContent.getWidth() / 2);
    flavorSelector.setBounds (flavorBounds.reduced (5, 0));

    // Carbonated toggle on right (vertical rocker - WIDER to fit "CARBONATED" text)
    auto toggleBounds = footerContent.withSizeKeepingCentre (120, 80);  // Increased width from 48 to 120
    carbonatedToggle.setBounds (toggleBounds);

    // FIZZ knob in center of remaining space
    bounds.removeFromBottom (10);  // Small gap above footer
    auto fizzBounds = bounds;
    fizzKnob.setBounds (fizzBounds);
}
