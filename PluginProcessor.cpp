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
    formatManager.registerBasicFormats();
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

bool SampleKeyFinderAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SampleKeyFinderAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

void SampleKeyFinderAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void SampleKeyFinderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

void SampleKeyFinderAudioProcessor::analyzeAudioFile (const juce::File& file)
{
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));
    if (reader != nullptr)
    {
        juce::AudioBuffer<float> buffer (static_cast<int>(reader->numChannels), static_cast<int>(reader->lengthInSamples));
        reader->read (&buffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        chromaExtractor.process (buffer);
        currentKey = juce::String (keyClassifier.detectKey (chromaExtractor.getChromaVector()));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SampleKeyFinderAudioProcessor();
}
