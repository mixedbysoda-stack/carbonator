#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/**
 * Soda Filter Theme System
 * Light & Dark mode support
 */
enum class ThemeType
{
    Light = 0,
    Dark = 1
};

namespace SodaColors
{
    // Base colors (constant)
    inline const juce::Colour sodaRed     { 0xffff3b30 };  // #ff3b30 - Primary red accent
    inline const juce::Colour sodaCream   { 0xfffff8f0 };  // #fff8f0 - Light cream
    inline const juce::Colour sodaDark    { 0xff0d0221 };  // #0d0221 - Dark navy
    inline const juce::Colour sodaMint    { 0xff4cd964 };  // #4cd964 - Fresh accent

    // Dark mode colors
    inline const juce::Colour darkBackground { 0xff1a0a2e };  // Deep purple-black
    inline const juce::Colour darkSurface    { 0xff2d1b4e };  // Dark purple
    inline const juce::Colour neonRed        { 0xffff0055 };  // Bright neon red
    inline const juce::Colour neonPurple     { 0xffbb00ff };  // Neon purple

    // Theme-aware color getter
    class Theme
    {
    public:
        static ThemeType currentTheme;

        static juce::Colour getBackground()
        {
            return currentTheme == ThemeType::Light ? sodaCream : darkBackground;
        }

        static juce::Colour getBorder()
        {
            return currentTheme == ThemeType::Light ? sodaRed : neonRed;
        }

        static juce::Colour getTextPrimary()
        {
            return currentTheme == ThemeType::Light ? juce::Colour(0xff2c2c2c) : sodaCream;
        }

        static juce::Colour getTextSecondary()
        {
            return currentTheme == ThemeType::Light ? juce::Colour(0xff666666) : juce::Colour(0xffaaaaaa);
        }

        static juce::Colour getTextHighlight()
        {
            return currentTheme == ThemeType::Light ? sodaRed : neonRed;
        }

        static juce::Colour getKnobBody()
        {
            return currentTheme == ThemeType::Light ? sodaDark : darkSurface;
        }

        static juce::Colour getKnobInner()
        {
            return currentTheme == ThemeType::Light ? juce::Colour(0xff1a0a2e) : darkBackground;
        }

        static juce::Colour getKnobBorder()
        {
            return currentTheme == ThemeType::Light ? juce::Colour(0xff2d1b4e) : neonPurple;
        }

        static juce::Colour getFooterBackground()
        {
            return currentTheme == ThemeType::Light ?
                juce::Colours::white.withAlpha(0.5f) :
                darkSurface.withAlpha(0.5f);
        }

        static juce::Colour getLiquidFill()
        {
            return currentTheme == ThemeType::Light ?
                sodaRed.withAlpha(0.1f) :
                neonRed.withAlpha(0.15f);
        }

        static juce::Colour getPulseRing()
        {
            return currentTheme == ThemeType::Light ? sodaRed : neonRed;
        }
    };

    // Legacy compatibility (kept for backward compatibility)
    inline const juce::Colour background     = sodaCream;
    inline const juce::Colour border         = sodaRed;
    inline const juce::Colour text           = juce::Colour(0xff2c2c2c);
    inline const juce::Colour knobBody       = sodaDark;
    inline const juce::Colour knobIndicator  = sodaRed;
    inline const juce::Colour accent         = sodaMint;
    inline const juce::Colour textPrimary    = juce::Colour(0xff2c2c2c);
    inline const juce::Colour textSecondary  = juce::Colour(0xff666666);
    inline const juce::Colour textHighlight  = sodaRed;
}
