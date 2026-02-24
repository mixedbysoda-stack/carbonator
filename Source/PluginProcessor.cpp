#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters/ParameterFactory.h"
#include <cmath>

//==============================================================================
SodaFilterAudioProcessor::SodaFilterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
      apvts (*this, &undoManager, "Parameters", ParameterFactory::createParameterLayout())
{
    // Create effects chain
    effectsChain = std::make_unique<EffectsChain>(apvts);
}

SodaFilterAudioProcessor::~SodaFilterAudioProcessor()
{
}

//==============================================================================
const juce::String SodaFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SodaFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SodaFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SodaFilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SodaFilterAudioProcessor::getTailLengthSeconds() const
{
    // Tail for modulated delays and chorus effects
    return 0.5;
}

int SodaFilterAudioProcessor::getNumPrograms()
{
    return 1;
}

int SodaFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SodaFilterAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String SodaFilterAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void SodaFilterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void SodaFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Prepare DSP
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    effectsChain->prepare (spec);

    // Report oversampling latency to host
    setLatencySamples (static_cast<int> (std::ceil (effectsChain->getLatencyInSamples())));
}

void SodaFilterAudioProcessor::releaseResources()
{
    // Free up any resources when playback stops
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SodaFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // We support stereo and mono
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SodaFilterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that didn't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Process through effects chain
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    effectsChain->process (context);

    // Update latency if quality mode changed
    int newLatency = static_cast<int> (std::ceil (effectsChain->getLatencyInSamples()));
    if (newLatency != getLatencySamples())
        setLatencySamples (newLatency);
}

//==============================================================================
bool SodaFilterAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SodaFilterAudioProcessor::createEditor()
{
    return new SodaFilterAudioProcessorEditor (*this);
}

//==============================================================================
void SodaFilterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save APVTS state
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SodaFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore APVTS state
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SodaFilterAudioProcessor();
}
