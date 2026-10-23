#pragma once

#include <JuceHeader.h>
#include <vector>

class ChromaExtractor
{
public:
    ChromaExtractor();
    ~ChromaExtractor();

    void prepare (double sampleRate, int samplesPerBlock);
    void process (const juce::AudioBuffer<float>& buffer);
    const std::vector<float>& getChromaVector() const;

private:
    std::vector<float> chromaVector;
};
