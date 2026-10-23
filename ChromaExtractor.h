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
    static constexpr int fftOrder = 11; // 2048 pontos
    static constexpr int fftSize = 1 << fftOrder;

    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

    std::vector<float> fifo;
    std::vector<float> fftData;
    int fifoIndex = 0;

    std::vector<float> chromaVector;
    double currentSampleRate = 44100.0;

    void pushNextSampleIntoFifo (float sample);
    void performFFT();
};
