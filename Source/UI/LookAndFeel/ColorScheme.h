#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/**
 * Soda Filter Color Palette
 * Retro soda can aesthetic
 */
namespace SodaColors
{
    // Primary colors
    inline const juce::Colour sodaRed     { 0xffff3b30 };  // #ff3b30 - Primary red accent
    inline const juce::Colour sodaCream   { 0xfffff8f0 };  // #fff8f0 - Cream background
    inline const juce::Colour sodaDark    { 0xff2c2c2c };  // #2c2c2c - Dark knob body
    inline const juce::Colour sodaMint    { 0xff4cd964 };  // #4cd964 - Optional fresh accent

    // UI elements
    inline const juce::Colour background     = sodaCream;
    inline const juce::Colour border         = sodaRed;
    inline const juce::Colour text           = sodaDark;
    inline const juce::Colour knobBody       = sodaDark;
    inline const juce::Colour knobIndicator  = sodaRed;
    inline const juce::Colour accent         = sodaMint;

    // Text colors
    inline const juce::Colour textPrimary    = sodaDark;
    inline const juce::Colour textSecondary  = juce::Colour { 0xff666666 };
    inline const juce::Colour textHighlight  = sodaRed;
}
