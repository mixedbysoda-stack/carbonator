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
    juce::ignoreUnused (slider, rotaryStartAngle, rotaryEndAngle);

    // Reduce bounds to fit the ridges (they extend outward by ~17% of radius)
    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (25.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centerX = bounds.getCentreX();
    auto centerY = bounds.getCentreY();

    // Calculate full rotation (0-360 degrees for bottle cap)
    auto rotation = sliderPos * juce::MathConstants<float>::twoPi;

    // Get theme-aware colors
    auto capColor = SodaColors::Theme::getBorder();

    // Draw scalloped edges (21 ridges like a real bottle cap)
    const int ridgeCount = 21;
    for (int i = 0; i < ridgeCount; ++i)
    {
        float angle = (i / static_cast<float>(ridgeCount)) * juce::MathConstants<float>::twoPi + rotation;
        bool isEven = (i % 2) == 0;

        // Ridge shape (trapezoid)
        juce::Path ridge;
        float ridgeWidth = radius * 0.12f;
        float ridgeHeight = radius * 0.15f;
        float ridgeRadius = radius * 1.02f;

        // Calculate ridge position
        float x1 = centerX + ridgeRadius * std::cos (angle - ridgeWidth / radius);
        float y1 = centerY + ridgeRadius * std::sin (angle - ridgeWidth / radius);
        float x2 = centerX + ridgeRadius * std::cos (angle + ridgeWidth / radius);
        float y2 = centerY + ridgeRadius * std::sin (angle + ridgeWidth / radius);

        float outerRadius = ridgeRadius + ridgeHeight;
        float x3 = centerX + outerRadius * std::cos (angle + ridgeWidth / (radius * 1.5f));
        float y3 = centerY + outerRadius * std::sin (angle + ridgeWidth / (radius * 1.5f));
        float x4 = centerX + outerRadius * std::cos (angle - ridgeWidth / (radius * 1.5f));
        float y4 = centerY + outerRadius * std::sin (angle - ridgeWidth / (radius * 1.5f));

        ridge.startNewSubPath (x1, y1);
        ridge.lineTo (x2, y2);
        ridge.lineTo (x3, y3);
        ridge.lineTo (x4, y4);
        ridge.closeSubPath();

        // Gradient for ridge (lighter on one side for 3D effect)
        g.setColour (isEven ? capColor : capColor.darker (0.15f));
        g.fillPath (ridge);
    }

    // Main bottle cap circle (88% of size)
    auto capBounds = bounds.reduced (bounds.getWidth() * 0.06f);

    // Radial gradient effect for cap (simulated with concentric circles)
    for (int i = 10; i >= 0; --i)
    {
        float alpha = 0.3f + (i / 10.0f) * 0.7f;
        float sizeFactor = 1.0f - (i * 0.02f);
        auto gradientBounds = capBounds.reduced (capBounds.getWidth() * (1.0f - sizeFactor) * 0.5f);

        g.setColour (capColor.withAlpha (alpha));
        g.fillEllipse (gradientBounds);
    }

    // Inner grooved ring (shadow)
    auto grooveBounds = bounds.reduced (bounds.getWidth() * 0.125f);
    g.setColour (juce::Colours::black.withAlpha (0.3f));
    g.drawEllipse (grooveBounds, 2.0f);

    // Center logo area (lighter)
    auto logoBounds = bounds.reduced (bounds.getWidth() * 0.20f);
    g.setColour (capColor.brighter (0.1f));
    g.fillEllipse (logoBounds);

    // Inner shadow for depth
    g.setColour (juce::Colours::black.withAlpha (0.15f));
    g.drawEllipse (logoBounds.reduced (1.0f), 1.5f);

    // White indicator notch (rectangular, rotates with knob)
    juce::Path indicator;
    float notchWidth = radius * 0.12f;
    float notchHeight = radius * 0.25f;
    float notchY = -radius * 0.65f;

    juce::AffineTransform transform = juce::AffineTransform::rotation (rotation, centerX, centerY);

    indicator.addRoundedRectangle (
        centerX - notchWidth / 2.0f,
        centerY + notchY,
        notchWidth,
        notchHeight,
        3.0f
    );

    g.setColour (juce::Colours::white.withAlpha (0.95f));
    g.fillPath (indicator, transform);

    // Notch shadow for depth
    g.setColour (juce::Colours::black.withAlpha (0.4f));
    g.strokePath (indicator, juce::PathStrokeType (0.5f), transform);

    // Highlight on top edge
    g.setColour (juce::Colours::white.withAlpha (0.3f));
    auto highlightArc = juce::Path();
    highlightArc.addCentredArc (centerX, centerY, radius * 0.88f, radius * 0.88f,
                                0.0f, -2.5f, -0.5f, true);
    g.strokePath (highlightArc, juce::PathStrokeType (2.0f));
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

    // Vertical rocker switch body (theme-aware)
    auto switchBounds = bounds.reduced (2.0f);
    g.setColour (SodaColors::Theme::getKnobBody());
    g.fillRoundedRectangle (switchBounds, 2.0f);

    // Border (theme-aware)
    g.setColour (SodaColors::Theme::getKnobBorder());
    g.drawRoundedRectangle (switchBounds, 2.0f, 2.0f);

    // Inner area (theme-aware)
    auto innerBounds = switchBounds.reduced (4.0f);
    g.setColour (SodaColors::Theme::getKnobInner());
    g.fillRoundedRectangle (innerBounds, 1.0f);

    // LED indicator at top (theme-aware color when ON)
    auto ledBounds = innerBounds.removeFromTop (8.0f).reduced (2.0f, 0.0f);
    auto ledColor = SodaColors::Theme::getBorder();

    if (isOn)
    {
        // Glowing LED
        g.setColour (ledColor);
        g.fillRoundedRectangle (ledBounds, 1.0f);

        // Glow effect
        g.setColour (ledColor.withAlpha (0.4f));
        auto glowBounds = ledBounds.expanded (2.0f);
        g.fillRoundedRectangle (glowBounds, 2.0f);
    }
    else
    {
        // Dim LED
        g.setColour (ledColor.withAlpha (0.2f));
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
