#include "PluginProcessor.h"
#include "PluginEditor.h"

SampleKeyFinderAudioProcessorEditor::SampleKeyFinderAudioProcessorEditor (SampleKeyFinderAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 200);

    titleLabel.setText ("KEY SCALE DETECTOR", juce::dontSendNotification);
    titleLabel.setFont (juce::FontOptions (18.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, juce::Colours::cyan);
    addAndMakeVisible (titleLabel);

    keyLabel.setText ("A aguardar áudio...", juce::dontSendNotification);
    keyLabel.setFont (juce::FontOptions (28.0f, juce::Font::bold));
    keyLabel.setJustificationType (juce::Justification::centred);
    keyLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (keyLabel);

    startTimerHz (10); // Atualiza o ecrã 10 vezes por segundo com a tonalidade detetada
}

SampleKeyFinderAudioProcessorEditor::~SampleKeyFinderAudioProcessorEditor()
{
    stopTimer();
}

void SampleKeyFinderAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1e1e24));
}

void SampleKeyFinderAudioProcessorEditor::resized()
{
    titleLabel.setBounds (0, 20, getWidth(), 30);
    keyLabel.setBounds (0, 80, getWidth(), 50);
}

void SampleKeyFinderAudioProcessorEditor::timerCallback()
{
    juce::String key = audioProcessor.getCurrentKey();
    if (key.isNotEmpty())
    {
        keyLabel.setText (key, juce::dontSendNotification);
    }
}
