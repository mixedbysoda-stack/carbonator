#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "UI/LookAndFeel/ColorScheme.h"

//==============================================================================
SodaFilterAudioProcessorEditor::SodaFilterAudioProcessorEditor (SodaFilterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), sodaPanel (p.getAPVTS())
{
    // Set look and feel
    setLookAndFeel (&sodaLAF);

    // Setup title label (LARGER for impact)
    titleLabel.setText ("SODA FILTER", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (36.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, SodaColors::sodaCream);
    titleLabel.setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (titleLabel);

    // Setup subtitle label (LARGER for readability)
    subtitleLabel.setText ("VINTAGE CARBONATED AUDIO", juce::dontSendNotification);
    subtitleLabel.setFont (juce::Font (13.0f, juce::Font::bold));
    subtitleLabel.setJustificationType (juce::Justification::centred);
    subtitleLabel.setColour (juce::Label::textColourId, SodaColors::sodaRed);
    addAndMakeVisible (subtitleLabel);

    // Setup theme toggle button (LARGER icon)
    themeToggleButton.setButtonText ("☀");  // Sun emoji for light mode
    themeToggleButton.onClick = [this]() { toggleTheme(); };
    addAndMakeVisible (themeToggleButton);

    // Add main panel
    addAndMakeVisible (sodaPanel);

    // Set default size (420 x 560, vertical like a soda can)
    setSize (420, 560);
    setResizable (true, true);
    setResizeLimits (350, 450, 600, 800);

    // Generate bubbles
    generateBubbles();

    // Start timer for animations (60 fps)
    startTimerHz (60);
}

void SodaFilterAudioProcessorEditor::generateBubbles()
{
    bubbles.clear();
    juce::Random random;

    // Generate 20 bubbles
    for (int i = 0; i < 20; ++i)
    {
        Bubble bubble;
        bubble.x = random.nextFloat();                     // Random horizontal position
        bubble.y = 1.0f + random.nextFloat() * 0.5f;       // Start below screen
        bubble.size = 5.0f + random.nextFloat() * 15.0f;   // Size: 5-20px
        bubble.speed = 0.002f + random.nextFloat() * 0.003f; // Speed variation
        bubble.delay = random.nextFloat() * 2.0f;          // Stagger start times
        bubble.lifetime = 0.0f;

        bubbles.push_back (bubble);
    }
}

void SodaFilterAudioProcessorEditor::updateBubbles()
{
    // Get FIZZ value to affect bubble speed
    auto* fizzParam = audioProcessor.getAPVTS().getRawParameterValue ("fizzAmount");
    float fizzValue = fizzParam != nullptr ? fizzParam->load() : 50.0f;
    float fizzPercent = fizzValue / 100.0f;

    // Speed multiplier based on fizz (more fizz = faster bubbles)
    float speedMultiplier = 0.5f + fizzPercent * 1.5f;

    for (auto& bubble : bubbles)
    {
        bubble.lifetime += 0.016f;  // ~60fps

        // Only move if past delay
        if (bubble.lifetime > bubble.delay)
        {
            bubble.y -= bubble.speed * speedMultiplier;

            // Reset when off screen
            if (bubble.y < -0.2f)
            {
                bubble.y = 1.0f + juce::Random::getSystemRandom().nextFloat() * 0.3f;
                bubble.x = juce::Random::getSystemRandom().nextFloat();
            }
        }
    }
}

juce::Colour SodaFilterAudioProcessorEditor::getBubbleColor()
{
    // Get current flavor
    auto* flavorParam = audioProcessor.getAPVTS().getRawParameterValue ("flavorType");
    int flavorIndex = flavorParam != nullptr ? static_cast<int>(flavorParam->load()) : 0;

    switch (flavorIndex)
    {
        case 0:  // Cherry - Super bright red
            return juce::Colour (0xffff0040);  // Hot pink-red, super vibrant

        case 1:  // Grape - Purple
            return juce::Colour (0xff9b59b6);

        case 2:  // Dirty Soda - Dark purple (lean color)
            return juce::Colour (0xff6a0dad);

        default:
            return juce::Colour (0xffff0040);
    }
}

SodaFilterAudioProcessorEditor::~SodaFilterAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void SodaFilterAudioProcessorEditor::timerCallback()
{
    // Update bubble positions
    auto* carbonatedParam = audioProcessor.getAPVTS().getRawParameterValue ("carbonated");
    bool isCarbonated = carbonatedParam != nullptr ? carbonatedParam->load() > 0.5f : true;

    if (isCarbonated)
        updateBubbles();

    // Repaint for liquid animation and bubbles
    repaint();
}

//==============================================================================
void SodaFilterAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Main background (theme-aware)
    g.fillAll (SodaColors::Theme::getBackground());

    // Border (theme-aware, rounded rectangle, 8px thick)
    auto borderBounds = bounds.reduced (4.0f);
    g.setColour (SodaColors::Theme::getBorder());
    g.drawRoundedRectangle (borderBounds, 32.0f, 8.0f);

    // Get FIZZ value for liquid fill effect
    auto* fizzParam = audioProcessor.getAPVTS().getRawParameterValue ("fizzAmount");
    float fizzValue = fizzParam != nullptr ? fizzParam->load() : 50.0f;
    float fizzPercent = fizzValue / 100.0f;

    // Liquid fill layer (rises from bottom based on FIZZ)
    auto innerBounds = bounds.reduced (12.0f);
    float liquidHeight = innerBounds.getHeight() * fizzPercent;
    auto liquidBounds = juce::Rectangle<float> (
        innerBounds.getX(),
        innerBounds.getBottom() - liquidHeight,
        innerBounds.getWidth(),
        liquidHeight
    );
    g.setColour (SodaColors::Theme::getLiquidFill());
    g.fillRect (liquidBounds);

    // Draw bubbles (only if carbonated)
    auto* carbonatedParam = audioProcessor.getAPVTS().getRawParameterValue ("carbonated");
    bool isCarbonated = carbonatedParam != nullptr ? carbonatedParam->load() > 0.5f : true;

    if (isCarbonated)
    {
        juce::Colour bubbleColor = getBubbleColor();

        for (const auto& bubble : bubbles)
        {
            // Only draw if past delay and on screen
            if (bubble.lifetime > bubble.delay && bubble.y >= 0.0f && bubble.y <= 1.0f)
            {
                float xPos = bubble.x * bounds.getWidth();
                float yPos = bubble.y * bounds.getHeight();

                // Calculate opacity based on FIZZ level and position
                float opacity = (fizzPercent / 2.0f + 0.1f) * (1.0f - bubble.y);
                opacity = juce::jlimit (0.1f, 0.8f, opacity);

                // Draw bubble with glow
                g.setColour (bubbleColor.withAlpha (opacity * 0.3f));
                g.fillEllipse (xPos - bubble.size * 1.5f, yPos - bubble.size * 1.5f,
                              bubble.size * 3.0f, bubble.size * 3.0f);

                g.setColour (bubbleColor.withAlpha (opacity));
                g.fillEllipse (xPos - bubble.size / 2.0f, yPos - bubble.size / 2.0f,
                              bubble.size, bubble.size);
            }
        }
    }

    // Draw noise texture overlay (simplified)
    g.setColour (juce::Colours::black.withAlpha (0.02f));
    for (int y = 0; y < getHeight(); y += 4)
    {
        for (int x = 0; x < getWidth(); x += 4)
        {
            if (juce::Random::getSystemRandom().nextFloat() > 0.5f)
                g.fillRect (x, y, 2, 2);
        }
    }

    // Glass reflection overlay (top-right gradient)
    juce::ColourGradient gradient (
        juce::Colours::white.withAlpha (0.4f),
        bounds.getRight(), 0.0f,
        juce::Colours::transparentWhite,
        bounds.getX(), bounds.getBottom(),
        false
    );
    g.setGradientFill (gradient);
    g.fillRoundedRectangle (borderBounds, 32.0f);

    // Draw title badge (red rounded pill with slight rotation)
    auto titleBounds = juce::Rectangle<float> (150.0f, 40.0f, 220.0f, 50.0f);
    titleBounds.setCentre (bounds.getCentreX(), 50.0f);

    g.saveState();
    g.addTransform (juce::AffineTransform::rotation (-0.035f, titleBounds.getCentreX(), titleBounds.getCentreY()));

    // Badge shadow
    g.setColour (juce::Colours::black.withAlpha (0.3f));
    g.fillRoundedRectangle (titleBounds.translated (0.0f, 3.0f), 25.0f);

    // Badge background (theme-aware)
    g.setColour (SodaColors::Theme::getBorder());
    g.fillRoundedRectangle (titleBounds, 25.0f);

    g.restoreState();
}

void SodaFilterAudioProcessorEditor::toggleTheme()
{
    // Toggle theme
    SodaColors::Theme::currentTheme =
        (SodaColors::Theme::currentTheme == ThemeType::Light) ?
        ThemeType::Dark : ThemeType::Light;

    // Update button text
    themeToggleButton.setButtonText (
        SodaColors::Theme::currentTheme == ThemeType::Light ? "☀" : "☾"
    );

    // Repaint everything
    repaint();
    sodaPanel.repaint();
}

void SodaFilterAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (20);

    // Theme toggle button (top-right corner, bigger for easier clicking)
    auto toggleBounds = juce::Rectangle<int> (bounds.getRight() - 50, bounds.getY() + 5, 45, 45);
    themeToggleButton.setBounds (toggleBounds);

    // Title badge (positioned manually in paint)
    titleLabel.setBounds (bounds.getX(), 35, bounds.getWidth(), 40);

    // Subtitle below badge
    subtitleLabel.setBounds (bounds.getX(), 80, bounds.getWidth(), 20);

    // Main panel takes remaining space
    auto panelBounds = bounds;
    panelBounds.removeFromTop (100);  // Space for header
    sodaPanel.setBounds (panelBounds);
}
