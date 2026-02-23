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

    // Dark navy outer circle (#0d0221)
    g.setColour (juce::Colour (0xff0d0221));
    g.fillEllipse (bounds);

    // Border (#2d1b4e)
    g.setColour (juce::Colour (0xff2d1b4e));
    g.drawEllipse (bounds, 2.0f);

    // Inner circle (#1a0a2e) - 50% size
    auto innerBounds = bounds.reduced (bounds.getWidth() * 0.25f);
    g.setColour (juce::Colour (0xff1a0a2e));
    g.fillEllipse (innerBounds);

    // Inner circle border
    g.setColour (juce::Colour (0xff2d1b4e));
    g.drawEllipse (innerBounds, 1.0f);

    // Calculate rotation angle (-135° to +135°, 270° total range)
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Draw indicator line (red, from center to edge)
    auto indicatorLength = radius * 0.4f;  // Shorter line, from center
    auto indicatorStartX = centerX;
    auto indicatorStartY = centerY;
    auto indicatorEndX = centerX + indicatorLength * std::cos (angle - juce::MathConstants<float>::halfPi);
    auto indicatorEndY = centerY + indicatorLength * std::sin (angle - juce::MathConstants<float>::halfPi);

    juce::Path indicator;
    indicator.addLineSegment (juce::Line<float> (indicatorStartX, indicatorStartY,
                                                 indicatorEndX, indicatorEndY),
                             4.0f);

    // Red indicator with glow
    g.setColour (SodaColors::sodaRed);
    g.fillPath (indicator);

    // Add glow effect
    g.setColour (SodaColors::sodaRed.withAlpha (0.5f));
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
    bool isOn = button.getToggleState();

    // Vertical rocker switch body (dark navy #0d0221)
    auto switchBounds = bounds.reduced (2.0f);
    g.setColour (juce::Colour (0xff0d0221));
    g.fillRoundedRectangle (switchBounds, 2.0f);

    // Border (#2d1b4e)
    g.setColour (juce::Colour (0xff2d1b4e));
    g.drawRoundedRectangle (switchBounds, 2.0f, 2.0f);

    // Inner area (#1a0a2e)
    auto innerBounds = switchBounds.reduced (4.0f);
    g.setColour (juce::Colour (0xff1a0a2e));
    g.fillRoundedRectangle (innerBounds, 1.0f);

    // LED indicator at top (red when ON)
    auto ledBounds = innerBounds.removeFromTop (8.0f).reduced (2.0f, 0.0f);
    if (isOn)
    {
        // Glowing red LED
        g.setColour (SodaColors::sodaRed);
        g.fillRoundedRectangle (ledBounds, 1.0f);

        // Glow effect
        g.setColour (SodaColors::sodaRed.withAlpha (0.4f));
        auto glowBounds = ledBounds.expanded (2.0f);
        g.fillRoundedRectangle (glowBounds, 2.0f);
    }
    else
    {
        // Dim LED
        g.setColour (SodaColors::sodaRed.withAlpha (0.2f));
        g.fillRoundedRectangle (ledBounds, 1.0f);
    }

    innerBounds.removeFromTop (4.0f);  // Gap after LED

    // Rocker/slider element (moves up when ON, down when OFF)
    auto rockerHeight = innerBounds.getHeight() * 0.6f;
    auto rockerBounds = innerBounds.withHeight (rockerHeight);

    if (!isOn)
        rockerBounds.translate (0.0f, innerBounds.getHeight() - rockerHeight);

    // Gradient for rocker (looks 3D)
    juce::ColourGradient gradient (
        juce::Colour (0xff333333), rockerBounds.getX(), rockerBounds.getY(),
        juce::Colour (0xff111111), rockerBounds.getX(), rockerBounds.getBottom(),
        false
    );
    g.setGradientFill (gradient);
    g.fillRect (rockerBounds);

    // Rocker border
    g.setColour (juce::Colour (0xff444444));
    g.drawRect (rockerBounds, 1.0f);

    // Highlight lines on rocker
    g.setColour (juce::Colours::white.withAlpha (0.1f));
    g.fillRect (rockerBounds.getX(), rockerBounds.getY() + 2.0f, rockerBounds.getWidth(), 1.0f);
    g.setColour (juce::Colours::white.withAlpha (0.05f));
    g.fillRect (rockerBounds.getX(), rockerBounds.getY() + 4.0f, rockerBounds.getWidth(), 1.0f);
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
