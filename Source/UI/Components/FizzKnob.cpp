#include "FizzKnob.h"
#include "Parameters/ParameterIDs.h"
#include "UI/LookAndFeel/ColorScheme.h"

FizzKnob::FizzKnob (juce::AudioProcessorValueTreeState& apvts)
    : apvts (apvts)
{
    // Setup FIZZ slider
    fizzSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    fizzSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    fizzSlider.setRotaryParameters (
        juce::MathConstants<float>::pi,         // startAngle: π (6 o'clock, 0%)
        3.0f * juce::MathConstants<float>::pi,  // endAngle: 3π (6 o'clock, 100%, full 360°)
        true  // stopAtEnd
    );
    addAndMakeVisible (fizzSlider);

    // Attach to parameter
    using namespace ParameterIDs::Filter;
    fizzAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, fizzAmount.getParamID(), fizzSlider
    );

    // Setup percentage label (LARGER for readability)
    percentageLabel.setFont (juce::Font (32.0f, juce::Font::bold));
    percentageLabel.setJustificationType (juce::Justification::centred);
    percentageLabel.setColour (juce::Label::textColourId, SodaColors::textPrimary);
    addAndMakeVisible (percentageLabel);

    // Setup title label (LARGER for readability)
    titleLabel.setText ("", juce::dontSendNotification);
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
        updateMoodWord();
    };

    // Initialize percentage and mood word
    fizzSlider.onValueChange();
    updateMoodWord();

    // Start animation timer (60 fps)
    startTimerHz (60);
}

void FizzKnob::paint (juce::Graphics& g)
{
    paintMoodText (g);

    auto knobBounds = fizzSlider.getBounds().toFloat();
    auto center = knobBounds.getCentre();
    auto radius = knobBounds.getWidth() / 2.0f;

    // Pulsing rings effect (always visible, more intense when interacting)
    float pulseIntensity = isInteracting ? 1.0f : 0.4f;

    // Draw 3 pulsing rings (theme-aware color)
    // Start further out so they don't overlap the bottle cap ridges
    auto ringColor = SodaColors::Theme::getPulseRing();

    for (int i = 0; i < 3; ++i)
    {
        float ringPhase = pulsePhase + (i * 0.3f);
        float ringSize = std::sin (ringPhase * juce::MathConstants<float>::pi * 2.0f) * 0.5f + 0.5f;
        float ringRadius = radius + (40.0f + i * 25.0f) * ringSize;  // Start at 40px instead of 30px
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
    // Position further out to not overlap bottle cap ridges
    float dashPhase = pulsePhase * 0.5f;
    g.setColour (ringColor.withAlpha (0.15f));

    juce::Path dashedRing;
    for (int i = 0; i < 12; ++i)
    {
        float angle = (i / 12.0f + dashPhase) * juce::MathConstants<float>::twoPi;
        float x1 = center.x + (radius + 55.0f) * std::cos (angle);  // Start at 55px
        float y1 = center.y + (radius + 55.0f) * std::sin (angle);
        float x2 = center.x + (radius + 65.0f) * std::cos (angle);  // End at 65px
        float y2 = center.y + (radius + 65.0f) * std::sin (angle);

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

    // Poll current flavor and update mood word if changed
    auto* flavorParam = apvts.getRawParameterValue ("flavorType");
    if (flavorParam != nullptr)
    {
        auto newFlavor = static_cast<FlavorType>(static_cast<int>(flavorParam->load()));
        if (newFlavor != currentFlavor)
        {
            currentFlavor = newFlavor;
            updateMoodWord();
        }
    }

    repaint();
}

void FizzKnob::resized()
{
    auto bounds = getLocalBounds();

    // Title at top (more space for larger text)
    titleArea = bounds.removeFromTop (40);
    titleLabel.setBounds (titleArea);

    // Large knob in center
    auto knobSize = juce::jmin (bounds.getWidth(), bounds.getHeight() - 60);
    auto knobBounds = bounds.removeFromTop (knobSize).withSizeKeepingCentre (knobSize, knobSize);
    fizzSlider.setBounds (knobBounds);

    // Percentage below knob (more space for larger text)
    percentageLabel.setBounds (bounds.removeFromTop (50));
}

void FizzKnob::updateMoodWord()
{
    moodWord = "FIZZ";
}

void FizzKnob::paintMoodText (juce::Graphics& g)
{
    if (titleArea.isEmpty())
        return;

    auto font = juce::Font (26.0f, juce::Font::bold);
    g.setFont (font);

    float fizz = static_cast<float>(fizzSlider.getValue()) / 100.0f;
    auto accent = SodaColors::Theme::getFlavorAccent();
    juce::String text = "FIZZ";

    switch (currentFlavor)
    {
        case FlavorType::Cola:
        {
            // Warm breathing glow + character wave (heat shimmer)
            float breathe = std::sin (pulsePhase * juce::MathConstants<float>::twoPi) * 0.5f + 0.5f;
            float glowSize = (2.0f + breathe * 6.0f) * fizz;

            // Pulsing warm glow behind text
            if (fizz > 0.05f)
            {
                for (float r = glowSize; r >= 1.0f; r -= 1.5f)
                {
                    float alpha = (1.0f - r / (glowSize + 1.0f)) * 0.15f * fizz;
                    g.setColour (accent.withAlpha (alpha));
                    g.drawText (text, titleArea.toFloat().expanded (r, r * 0.5f), juce::Justification::centred, false);
                }
            }

            // Each character bobs up/down in a wave
            float totalW = font.getStringWidthFloat (text);
            float startX = titleArea.getCentreX() - totalW / 2.0f;
            float baseY = static_cast<float>(titleArea.getY());
            float h = static_cast<float>(titleArea.getHeight());

            for (int i = 0; i < text.length(); ++i)
            {
                float charPhase = pulsePhase + i * 0.15f;
                float yOff = std::sin (charPhase * juce::MathConstants<float>::twoPi * 1.5f) * fizz * 3.0f;
                auto ch = text.substring (i, i + 1);
                float cw = font.getStringWidthFloat (ch);

                g.setColour (accent.interpolatedWith (juce::Colours::white, breathe * fizz * 0.3f));
                g.drawText (ch, juce::Rectangle<float>(startX, baseY + yOff, cw + 1.0f, h),
                            juce::Justification::centred, false);
                startX += cw;
            }
            break;
        }

        case FlavorType::Cherry:
        {
            // Glossy bloom + sweeping sheen highlight
            float sheenPos = std::fmod (pulsePhase * 2.0f, 1.0f);
            float bloomPulse = std::sin (pulsePhase * juce::MathConstants<float>::twoPi * 1.5f) * 0.5f + 0.5f;

            // Bloom glow behind text
            float bloomSize = (1.0f + bloomPulse * 4.0f) * fizz;
            g.setColour (accent.withAlpha (0.12f * fizz));
            g.drawText (text, titleArea.toFloat().expanded (bloomSize, bloomSize * 0.5f), juce::Justification::centred, false);

            // Each character lit by a sweeping sheen
            float totalW = font.getStringWidthFloat (text);
            float startX = titleArea.getCentreX() - totalW / 2.0f;
            float baseY = static_cast<float>(titleArea.getY());
            float h = static_cast<float>(titleArea.getHeight());

            for (int i = 0; i < text.length(); ++i)
            {
                auto ch = text.substring (i, i + 1);
                float cw = font.getStringWidthFloat (ch);
                float charCenter = (startX + cw / 2.0f - titleArea.getX()) / static_cast<float>(titleArea.getWidth());

                float distToSheen = std::abs (charCenter - sheenPos);
                float sheenBright = juce::jmax (0.0f, 1.0f - distToSheen * 4.0f) * fizz;

                auto charColor = accent.brighter (0.2f).interpolatedWith (juce::Colours::white, sheenBright * 0.6f);
                g.setColour (charColor);
                g.drawText (ch, juce::Rectangle<float>(startX, baseY, cw + 1.0f, h),
                            juce::Justification::centred, false);
                startX += cw;
            }
            break;
        }

        case FlavorType::Grape:
        {
            // VHS tape distortion: chromatic aberration + jitter + glitch + scanlines
            float jitterX = std::sin (pulsePhase * juce::MathConstants<float>::twoPi * 7.3f) * fizz * 3.0f;
            float jitterY = std::cos (pulsePhase * juce::MathConstants<float>::twoPi * 11.1f) * fizz * 1.5f;

            float aberration = fizz * 3.0f;

            // Red channel offset left
            g.setColour (juce::Colour (0xffff4444).withAlpha (0.4f * fizz));
            g.drawText (text, titleArea.toFloat().translated (jitterX - aberration, jitterY), juce::Justification::centred, false);

            // Blue channel offset right
            g.setColour (juce::Colour (0xff4444ff).withAlpha (0.4f * fizz));
            g.drawText (text, titleArea.toFloat().translated (jitterX + aberration, jitterY), juce::Justification::centred, false);

            // Glitch: occasional horizontal shift
            float glitchChance = std::sin (pulsePhase * juce::MathConstants<float>::twoPi * 13.7f);
            float glitchOffset = (glitchChance > 0.85f) ? fizz * 8.0f : 0.0f;

            // Main text
            g.setColour (accent.withAlpha (0.9f));
            g.drawText (text, titleArea.toFloat().translated (jitterX + glitchOffset, jitterY), juce::Justification::centred, false);

            // Scanline overlay
            if (fizz > 0.15f)
            {
                g.setColour (juce::Colours::black.withAlpha (0.08f * fizz));
                for (int y = titleArea.getY(); y < titleArea.getBottom(); y += 3)
                    g.fillRect (titleArea.getX(), y, titleArea.getWidth(), 1);
            }
            break;
        }

        case FlavorType::LemonLime:
        {
            // Electric neon glow + character flash + spark particles
            float pulse = std::sin (pulsePhase * juce::MathConstants<float>::twoPi * 3.0f) * 0.5f + 0.5f;

            // Neon glow halo
            float glowSize = (1.0f + pulse * 3.0f) * fizz;
            for (float r = glowSize; r >= 1.0f; r -= 1.0f)
            {
                float alpha = (1.0f - r / (glowSize + 1.0f)) * 0.2f * fizz;
                g.setColour (accent.withAlpha (alpha));
                g.drawText (text, titleArea.toFloat().expanded (r, r * 0.5f), juce::Justification::centred, false);
            }

            // Characters with individual flash cycles
            float totalW = font.getStringWidthFloat (text);
            float startX = titleArea.getCentreX() - totalW / 2.0f;
            float baseY = static_cast<float>(titleArea.getY());
            float h = static_cast<float>(titleArea.getHeight());

            for (int i = 0; i < text.length(); ++i)
            {
                auto ch = text.substring (i, i + 1);
                float cw = font.getStringWidthFloat (ch);

                float charFlash = std::sin ((pulsePhase + i * 0.25f) * juce::MathConstants<float>::twoPi * 4.0f);
                bool isFlashing = charFlash > 0.7f && fizz > 0.3f;

                g.setColour (isFlashing ? juce::Colours::white : accent);
                g.drawText (ch, juce::Rectangle<float>(startX, baseY, cw + 1.0f, h),
                            juce::Justification::centred, false);
                startX += cw;
            }

            // Spark particles
            if (fizz > 0.1f)
            {
                juce::Random sparkRng (static_cast<juce::int64>(pulsePhase * 12));
                int sparkCount = static_cast<int>(fizz * 8);
                for (int i = 0; i < sparkCount; ++i)
                {
                    float sx = titleArea.getX() + sparkRng.nextFloat() * titleArea.getWidth();
                    float sy = titleArea.getY() + sparkRng.nextFloat() * titleArea.getHeight();
                    float sparkSize = 1.0f + sparkRng.nextFloat() * 2.5f;
                    g.setColour (accent.brighter (0.5f).withAlpha (sparkRng.nextFloat() * fizz * 0.8f));
                    g.fillEllipse (sx, sy, sparkSize, sparkSize);
                }
            }
            break;
        }

        case FlavorType::OrangeCream:
        {
            // Stereo spread: dual ghost text breathes apart + warm center
            float breathe = std::sin (pulsePhase * juce::MathConstants<float>::twoPi * 0.8f) * 0.5f + 0.5f;
            float spread = (2.0f + breathe * 4.0f) * fizz;

            // Left ghost (stereo L)
            g.setColour (accent.withAlpha (0.25f * fizz));
            g.drawText (text, titleArea.toFloat().translated (-spread, 0), juce::Justification::centred, false);

            // Right ghost (stereo R)
            g.setColour (accent.withAlpha (0.25f * fizz));
            g.drawText (text, titleArea.toFloat().translated (spread, 0), juce::Justification::centred, false);

            // Warm center glow
            float glowAlpha = 0.08f + breathe * 0.1f * fizz;
            g.setColour (accent.withAlpha (glowAlpha));
            g.drawText (text, titleArea.toFloat().expanded (2.0f, 1.0f), juce::Justification::centred, false);

            // Main text
            g.setColour (accent.interpolatedWith (juce::Colours::white, breathe * fizz * 0.2f));
            g.drawText (text, titleArea, juce::Justification::centred, false);
            break;
        }

        default:
        {
            g.setColour (SodaColors::Theme::getTextSecondary());
            g.drawText (text, titleArea, juce::Justification::centred, false);
            break;
        }
    }
}
