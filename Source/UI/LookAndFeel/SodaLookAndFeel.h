#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "ColorScheme.h"

/**
 * Custom Look and Feel for Soda Filter
 * Retro/vintage aesthetic with rounded edges
 */
class SodaLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SodaLookAndFeel();

    // Rotary slider (knob) rendering
    void drawRotarySlider (juce::Graphics& g,
                          int x, int y, int width, int height,
                          float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    // ComboBox rendering
    void drawComboBox (juce::Graphics& g, int width, int height,
                      bool isButtonDown, int buttonX, int buttonY,
                      int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    // ToggleButton rendering
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;

    // Label rendering
    void drawLabel (juce::Graphics& g, juce::Label& label) override;
};
