#include "SodaLookAndFeel.h"

SodaLookAndFeel::SodaLookAndFeel()
{
    // Set default colors
    setColour (juce::ResizableWindow::backgroundColourId, SodaColors::background);
    setColour (juce::Label::textColourId, SodaColors::textPrimary);
    setColour (juce::ComboBox::backgroundColourId, SodaColors::background);
    setColour (juce::ComboBox::textColourId, SodaColors::textPrimary);
    setColour (juce::ComboBox::outlineColourId, SodaColors::border);
}

void SodaLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                       int x, int y, int width, int height,
                                       float sliderPos,
                                       float rotaryStartAngle, float rotaryEndAngle,
                                       juce::Slider& slider)
{
    juce::ignoreUnused (slider);

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centerX = bounds.getCentreX();
    auto centerY = bounds.getCentreY();

    // Draw knob body (dark circle)
    g.setColour (SodaColors::knobBody);
    g.fillEllipse (bounds);

    // Draw knob outline (red)
    g.setColour (SodaColors::border);
    g.drawEllipse (bounds, 2.0f);

    // Draw indicator line (red)
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineLength = radius * 0.7f;

    juce::Path indicator;
    indicator.addLineSegment (juce::Line<float> (centerX, centerY,
                                                 centerX + lineLength * std::cos (angle - juce::MathConstants<float>::halfPi),
                                                 centerY + lineLength * std::sin (angle - juce::MathConstants<float>::halfPi)),
                             3.0f);

    g.setColour (SodaColors::knobIndicator);
    g.fillPath (indicator);
}

void SodaLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height,
                                   bool isButtonDown, int buttonX, int buttonY,
                                   int buttonW, int buttonH,
                                   juce::ComboBox& box)
{
    juce::ignoreUnused (buttonX, buttonY, buttonW, buttonH);

    auto bounds = juce::Rectangle<int> (0, 0, width, height).toFloat();

    // Background
    g.setColour (SodaColors::background);
    g.fillRoundedRectangle (bounds, 4.0f);

    // Border
    g.setColour (isButtonDown ? SodaColors::border.brighter() : SodaColors::border);
    g.drawRoundedRectangle (bounds.reduced (0.5f), 4.0f, 2.0f);

    // Arrow (downward triangle)
    auto arrowZone = bounds.removeFromRight (20.0f).reduced (5.0f);
    juce::Path arrow;
    arrow.addTriangle (arrowZone.getX(), arrowZone.getY() + 2.0f,
                      arrowZone.getRight(), arrowZone.getY() + 2.0f,
                      arrowZone.getCentreX(), arrowZone.getBottom() - 2.0f);

    g.setColour (box.findColour (juce::ComboBox::arrowColourId).withAlpha (box.isEnabled() ? 1.0f : 0.3f));
    g.fillPath (arrow);
}

void SodaLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                       bool shouldDrawButtonAsHighlighted,
                                       bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused (shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

    auto bounds = button.getLocalBounds().toFloat();
    auto toggleWidth = bounds.getWidth();
    auto toggleHeight = bounds.getHeight();

    // Draw slide toggle background
    auto trackBounds = bounds.withTrimmedLeft (60.0f).reduced (0.0f, 10.0f);
    g.setColour (button.getToggleState() ? SodaColors::sodaMint : SodaColors::textSecondary);
    g.fillRoundedRectangle (trackBounds, trackBounds.getHeight() / 2.0f);

    // Draw toggle circle
    auto circleSize = trackBounds.getHeight() - 4.0f;
    auto circleX = button.getToggleState() ?
                   trackBounds.getRight() - circleSize - 2.0f :
                   trackBounds.getX() + 2.0f;

    g.setColour (SodaColors::background);
    g.fillEllipse (circleX, trackBounds.getY() + 2.0f, circleSize, circleSize);

    // Draw label text on the left
    g.setColour (SodaColors::textPrimary);
    g.setFont (14.0f);
    auto textBounds = bounds.withWidth (50.0f);
    g.drawText (button.getButtonText(), textBounds, juce::Justification::centredLeft);
}

void SodaLookAndFeel::drawLabel (juce::Graphics& g, juce::Label& label)
{
    g.fillAll (label.findColour (juce::Label::backgroundColourId));

    if (!label.isBeingEdited())
    {
        auto alpha = label.isEnabled() ? 1.0f : 0.5f;
        const juce::Font font (getLabelFont (label));

        g.setColour (label.findColour (juce::Label::textColourId).withMultipliedAlpha (alpha));
        g.setFont (font);

        auto textArea = getLabelBorderSize (label).subtractedFrom (label.getLocalBounds());

        g.drawFittedText (label.getText(), textArea, label.getJustificationType(),
                         juce::jmax (1, (int) ((float) textArea.getHeight() / font.getHeight())),
                         label.getMinimumHorizontalScale());

        g.setColour (label.findColour (juce::Label::outlineColourId).withMultipliedAlpha (alpha));
    }
}
