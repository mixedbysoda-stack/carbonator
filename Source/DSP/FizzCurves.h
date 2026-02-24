#pragma once

#include <cmath>
#include <juce_core/juce_core.h>

/**
 * Nonlinear Fizz mapping curves for Carbonator v2.0
 * Replaces linear jmap() calls with musical response curves.
 * All functions take fizz (0-1) and return a value in [min, max].
 */
namespace FizzCurves
{
    /** Exponential curve: slow start, fast finish.
     *  Use for drive, ratio, saturation — parameters that should ramp up late.
     *  curve > 1.0 = more exponential, 1.0 = linear. */
    inline float exponential (float fizz, float min, float max, float curve = 2.0f)
    {
        float shaped = std::pow (fizz, curve);
        return min + shaped * (max - min);
    }

    /** Logarithmic curve: fast start, slow finish.
     *  Use for EQ gains, cutoff — parameters that should be responsive early.
     *  curve > 1.0 = more logarithmic, 1.0 = linear. */
    inline float logarithmic (float fizz, float min, float max, float curve = 2.0f)
    {
        float shaped = 1.0f - std::pow (1.0f - fizz, curve);
        return min + shaped * (max - min);
    }

    /** S-curve (Hermite smoothstep): gentle at extremes, steep in middle.
     *  Use for blend, width, detune — parameters most musical mid-range. */
    inline float sCurve (float fizz, float min, float max)
    {
        // Hermite smoothstep: 3x^2 - 2x^3
        float shaped = fizz * fizz * (3.0f - 2.0f * fizz);
        return min + shaped * (max - min);
    }

    /** Linear mapping (same as juce::jmap).
     *  Preserved where already correct. */
    inline float linear (float fizz, float min, float max)
    {
        return min + fizz * (max - min);
    }
}
