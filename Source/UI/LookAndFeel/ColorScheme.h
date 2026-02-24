#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Parameters/ParameterIDs.h"

/**
 * Soda Filter Theme System
 * Light & Dark mode support + per-flavor color theming
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

    // Dark mode colors (legacy, kept for non-accent uses)
    inline const juce::Colour darkBackground { 0xff1a0a2e };  // Deep purple-black
    inline const juce::Colour darkSurface    { 0xff2d1b4e };  // Dark purple
    inline const juce::Colour neonRed        { 0xffff0055 };  // Bright neon red
    inline const juce::Colour neonPurple     { 0xffbb00ff };  // Neon purple

    // Per-flavor accent colors (light mode)
    inline const juce::Colour colaAccentLight        { 0xffc67c4e };  // Caramel
    inline const juce::Colour cherryAccentLight      { 0xffff3b30 };  // Red
    inline const juce::Colour grapeAccentLight       { 0xff9b59b6 };  // Purple
    inline const juce::Colour lemonLimeAccentLight   { 0xff7ec850 };  // Citrus green
    inline const juce::Colour orangeCreamAccentLight { 0xffff8c42 };  // Warm orange

    // Per-flavor accent colors (dark/neon mode)
    inline const juce::Colour colaAccentDark        { 0xffe8944a };  // Warm amber
    inline const juce::Colour cherryAccentDark      { 0xffff0055 };  // Neon red
    inline const juce::Colour grapeAccentDark       { 0xffbb00ff };  // Neon purple
    inline const juce::Colour lemonLimeAccentDark   { 0xff39ff14 };  // Neon green
    inline const juce::Colour orangeCreamAccentDark { 0xffff6b1a };  // Neon orange

    // Per-flavor dark backgrounds
    inline const juce::Colour colaDarkBg        { 0xff1e0f08 };  // Dark brown-black
    inline const juce::Colour cherryDarkBg      { 0xff1a0a2e };  // Purple-black (original)
    inline const juce::Colour grapeDarkBg       { 0xff120a2e };  // Deeper purple
    inline const juce::Colour lemonLimeDarkBg   { 0xff0a1e08 };  // Dark green-black
    inline const juce::Colour orangeCreamDarkBg { 0xff1e1208 };  // Dark amber-black

    // Theme-aware color getter
    class Theme
    {
    public:
        static ThemeType currentTheme;
        static FlavorType currentFlavor;

        static juce::Colour getFlavorAccent()
        {
            if (currentTheme == ThemeType::Light)
            {
                switch (currentFlavor)
                {
                    case FlavorType::Cola:        return colaAccentLight;
                    case FlavorType::Cherry:      return cherryAccentLight;
                    case FlavorType::Grape:       return grapeAccentLight;
                    case FlavorType::LemonLime:   return lemonLimeAccentLight;
                    case FlavorType::OrangeCream: return orangeCreamAccentLight;
                    default:                      return colaAccentLight;
                }
            }
            else
            {
                switch (currentFlavor)
                {
                    case FlavorType::Cola:        return colaAccentDark;
                    case FlavorType::Cherry:      return cherryAccentDark;
                    case FlavorType::Grape:       return grapeAccentDark;
                    case FlavorType::LemonLime:   return lemonLimeAccentDark;
                    case FlavorType::OrangeCream: return orangeCreamAccentDark;
                    default:                      return colaAccentDark;
                }
            }
        }

        static juce::Colour getBackground()
        {
            if (currentTheme == ThemeType::Light)
                return sodaCream;

            switch (currentFlavor)
            {
                case FlavorType::Cola:        return colaDarkBg;
                case FlavorType::Cherry:      return cherryDarkBg;
                case FlavorType::Grape:       return grapeDarkBg;
                case FlavorType::LemonLime:   return lemonLimeDarkBg;
                case FlavorType::OrangeCream: return orangeCreamDarkBg;
                default:                      return cherryDarkBg;
            }
        }

        static juce::Colour getBorder()
        {
            return getFlavorAccent();
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
            return getFlavorAccent();
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
            return currentTheme == ThemeType::Light ? juce::Colour(0xff2d1b4e) : getFlavorAccent();
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
                getFlavorAccent().withAlpha(0.1f) :
                getFlavorAccent().withAlpha(0.15f);
        }

        static juce::Colour getPulseRing()
        {
            return getFlavorAccent();
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
