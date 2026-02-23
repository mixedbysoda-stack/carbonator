#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "Components/FizzKnob.h"
#include "Components/FlavorSelector.h"
#include "Components/CarbonatedToggle.h"

/**
 * Main panel containing all UI components
 * Vertical layout with FIZZ knob, flavor controls, etc.
 */
class SodaPanel : public juce::Component
{
public:
    SodaPanel (juce::AudioProcessorValueTreeState& apvts);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    FizzKnob fizzKnob;
    FlavorSelector flavorSelector;
    CarbonatedToggle carbonatedToggle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SodaPanel)
};
