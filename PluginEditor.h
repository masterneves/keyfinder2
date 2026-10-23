#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SampleKeyFinderAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                              public juce::FileDragAndDropTarget
{
public:
    SampleKeyFinderAudioProcessorEditor (SampleKeyFinderAudioProcessor&);
    ~SampleKeyFinderAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;

private:
    SampleKeyFinderAudioProcessor& audioProcessor;
    juce::Label resultLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleKeyFinderAudioProcessorEditor)
};
