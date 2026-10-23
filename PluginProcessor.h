#pragma once
#include <JuceHeader.h>
#include "ChromaExtractor.h"
#include "KeyClassifier.h"

class SampleKeyFinderAudioProcessor   public juceAudioProcessor
{
public
    SampleKeyFinderAudioProcessor();
    ~SampleKeyFinderAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juceAudioBufferfloat&, juceMidiBuffer&) override;

    juceAudioProcessorEditor createEditor() override;
    bool hasEditor() const override { return true; }

    const juceString getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juceString getProgramName (int) override { return {}; }
    void changeProgramName (int, const juceString&) override {}

    void getStateInformation (juceMemoryBlock& destData) override {}
    void setStateInformation (const void data, int sizeInBytes) override {}

    void analyzeAudioFile(const juceFile& file);
    juceString getDetectedKey() const { return detectedKey; }

private
    juceAudioFormatManager formatManager;
    ChromaExtractor chromaExtractor;
    KeyClassifier keyClassifier;
    juceString detectedKey { Aguardando sample... };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleKeyFinderAudioProcessor)
};
