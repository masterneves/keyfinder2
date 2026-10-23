#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class SampleKeyFinderAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                              private juce::Timer
{
public:
    SampleKeyFinderAudioProcessorEditor (SampleKeyFinderAudioProcessor&);
    ~SampleKeyFinderAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    SampleKeyFinderAudioProcessor& audioProcessor;

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Label keyLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleKeyFinderAudioProcessorEditor)
};
