#include "PluginProcessor.h"
#include "PluginEditor.h"

SampleKeyFinderAudioProcessor::SampleKeyFinderAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

SampleKeyFinderAudioProcessor::~SampleKeyFinderAudioProcessor()
{
}

void SampleKeyFinderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    chromaExtractor.prepare (sampleRate, samplesPerBlock);
}

void SampleKeyFinderAudioProcessor::releaseResources()
{
}

bool SampleKeyFinderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void SampleKeyFinderAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    chromaExtractor.process (buffer);
    std::string key = keyClassifier.detectKey (chromaExtractor.getChromaVector());
    currentKey = juce::String (key);
}

juce::AudioProcessorEditor* SampleKeyFinderAudioProcessor::createEditor()
{
    return new SampleKeyFinderAudioProcessorEditor (*this);
}

void SampleKeyFinderAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void SampleKeyFinderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SampleKeyFinderAudioProcessor();
}
