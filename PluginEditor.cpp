#include "PluginProcessor.h"
#include "PluginEditor.h"

SampleKeyFinderAudioProcessorEditor::SampleKeyFinderAudioProcessorEditor (SampleKeyFinderAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (420, 240);

    // Título
    titleLabel.setText ("KEY SCALE DETECTOR", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, juce::Colour (0xff00f0ff));
    addAndMakeVisible (titleLabel);

    // Subtítulo
    subtitleLabel.setText ("REAL-TIME AUDIO ANALYZER", juce::dontSendNotification);
    subtitleLabel.setFont (juce::Font (10.0f, juce::Font::plain));
    subtitleLabel.setJustificationType (juce::Justification::centred);
    subtitleLabel.setColour (juce::Label::textColourId, juce::Colour (0xff8a8a93));
    addAndMakeVisible (subtitleLabel);

    // Tonalidade Detetada
    keyLabel.setText ("A AGUARDAR ÁUDIO...", juce::dontSendNotification);
    keyLabel.setFont (juce::Font (28.0f, juce::Font::bold));
    keyLabel.setJustificationType (juce::Justification::centred);
    keyLabel.setColour (juce::Label::textColourId, juce::Colour (0xffffffff));
    addAndMakeVisible (keyLabel);

    startTimerHz (15);
}

SampleKeyFinderAudioProcessorEditor::~SampleKeyFinderAudioProcessorEditor()
{
    stopTimer();
}

void SampleKeyFinderAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient bgGradient (juce::Colour (0xff232526), getWidth() * 0.5f, getHeight() * 0.5f,
                                     juce::Colour (0xff0f1011), 0.0f, 0.0f, true);
    g.setGradientFill (bgGradient);
    g.fillAll();

    g.setColour (juce::Colour (0xff00f0ff).withAlpha (0.3f));
    g.drawRect (getLocalBounds(), 1);

    juce::Rectangle<float> cardArea (30.0f, 75.0f, static_cast<float> (getWidth() - 60), 120.0f);
    g.setColour (juce::Colour (0xff18191c));
    g.fillRoundedRectangle (cardArea, 12.0f);

    g.setColour (juce::Colour (0xff00f0ff).withAlpha (0.6f));
    g.drawRoundedRectangle (cardArea, 12.0f, 1.5f);
}

void SampleKeyFinderAudioProcessorEditor::resized()
{
    titleLabel.setBounds (0, 18, getWidth(), 22);
    subtitleLabel.setBounds (0, 40, getWidth(), 16);
    keyLabel.setBounds (30, 75, getWidth() - 60, 120);
}

void SampleKeyFinderAudioProcessorEditor::timerCallback()
{
    juce::String key = audioProcessor.getCurrentKey();
    if (key.isNotEmpty())
    {
        keyLabel.setText (key, juce::dontSendNotification);
    }
}
