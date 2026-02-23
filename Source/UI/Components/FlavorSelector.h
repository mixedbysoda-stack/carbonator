#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * Flavor selector component
 * Allows choosing between Cherry, Grape, Dirty Soda
 */
class FlavorSelector : public juce::Component
{
public:
    FlavorSelector (juce::AudioProcessorValueTreeState& apvts);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    juce::Label titleLabel;
    juce::ComboBox flavorCombo;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> flavorAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlavorSelector)
};
