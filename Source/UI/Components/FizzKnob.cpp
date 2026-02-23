#include "FizzKnob.h"
#include "Parameters/ParameterIDs.h"
#include "UI/LookAndFeel/ColorScheme.h"

FizzKnob::FizzKnob (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts)
{
    // Setup FIZZ slider
    fizzSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    fizzSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (fizzSlider);

    // Attach to parameter
    using namespace ParameterIDs::Filter;
    fizzAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, fizzAmount.getParamID(), fizzSlider
    );

    // Setup percentage label
    percentageLabel.setFont (juce::Font (24.0f, juce::Font::bold));
    percentageLabel.setJustificationType (juce::Justification::centred);
    percentageLabel.setColour (juce::Label::textColourId, SodaColors::textPrimary);
    addAndMakeVisible (percentageLabel);

    // Setup title label
    titleLabel.setText ("FIZZ", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, SodaColors::textSecondary);
    addAndMakeVisible (titleLabel);

    // Track interaction state
    fizzSlider.onDragStart = [this]()
    {
        isInteracting = true;
    };

    fizzSlider.onDragEnd = [this]()
    {
        isInteracting = false;
    };

    // Update percentage on value change
    fizzSlider.onValueChange = [this]()
    {
        int percent = static_cast<int>(fizzSlider.getValue());
        percentageLabel.setText (juce::String (percent) + "%", juce::dontSendNotification);

        // Trigger pulse on value change
        float currentValue = fizzSlider.getValue();
        if (std::abs (currentValue - lastValue) > 0.5f)
        {
            pulsePhase = 0.0f;  // Reset pulse
        }
        lastValue = currentValue;
    };

    // Initialize percentage
    fizzSlider.onValueChange();

    // Start animation timer (60 fps)
    startTimerHz (60);
}

void FizzKnob::paint (juce::Graphics& g)
{
    auto knobBounds = fizzSlider.getBounds().toFloat();
    auto center = knobBounds.getCentre();
    auto radius = knobBounds.getWidth() / 2.0f;

    // Pulsing rings effect (always visible, more intense when interacting)
    float pulseIntensity = isInteracting ? 1.0f : 0.4f;

    // Draw 3 pulsing rings (theme-aware color)
    auto ringColor = SodaColors::Theme::getPulseRing();

    for (int i = 0; i < 3; ++i)
    {
        float ringPhase = pulsePhase + (i * 0.3f);
        float ringSize = std::sin (ringPhase * juce::MathConstants<float>::pi * 2.0f) * 0.5f + 0.5f;
        float ringRadius = radius + (30.0f + i * 20.0f) * ringSize;
        float ringOpacity = (1.0f - ringSize) * 0.3f * pulseIntensity;

        g.setColour (ringColor.withAlpha (ringOpacity));
        g.drawEllipse (center.x - ringRadius, center.y - ringRadius,
                      ringRadius * 2.0f, ringRadius * 2.0f, 3.0f);
    }

    // Glow effect around knob when interacting
    if (isInteracting)
    {
        float glowSize = std::sin (pulsePhase * juce::MathConstants<float>::pi * 4.0f) * 0.3f + 0.7f;
        float glowRadius = radius * (1.0f + glowSize * 0.4f);
        float glowOpacity = 0.2f * glowSize;

        g.setColour (ringColor.withAlpha (glowOpacity));
        g.fillEllipse (center.x - glowRadius, center.y - glowRadius,
                      glowRadius * 2.0f, glowRadius * 2.0f);
    }

    // Dashed spinning ring (subtle always-on animation)
    float dashPhase = pulsePhase * 0.5f;
    g.setColour (ringColor.withAlpha (0.15f));

    juce::Path dashedRing;
    for (int i = 0; i < 12; ++i)
    {
        float angle = (i / 12.0f + dashPhase) * juce::MathConstants<float>::twoPi;
        float x1 = center.x + (radius + 45.0f) * std::cos (angle);
        float y1 = center.y + (radius + 45.0f) * std::sin (angle);
        float x2 = center.x + (radius + 55.0f) * std::cos (angle);
        float y2 = center.y + (radius + 55.0f) * std::sin (angle);

        dashedRing.addLineSegment (juce::Line<float> (x1, y1, x2, y2), 2.0f);
    }

    g.fillPath (dashedRing);
}

void FizzKnob::timerCallback()
{
    // Update pulse phase
    pulsePhase += 0.02f;
    if (pulsePhase > 1.0f)
        pulsePhase -= 1.0f;

    repaint();
}

void FizzKnob::resized()
{
    auto bounds = getLocalBounds();

    // Title at top
    titleLabel.setBounds (bounds.removeFromTop (30));

    // Large knob in center
    auto knobSize = juce::jmin (bounds.getWidth(), bounds.getHeight() - 40);
    auto knobBounds = bounds.removeFromTop (knobSize).withSizeKeepingCentre (knobSize, knobSize);
    fizzSlider.setBounds (knobBounds);

    // Percentage below knob
    percentageLabel.setBounds (bounds.removeFromTop (40));
}
