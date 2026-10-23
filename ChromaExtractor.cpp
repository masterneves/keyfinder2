#include "ChromaExtractor.h"
#include <cmath>

ChromaExtractor::ChromaExtractor()
    : forwardFFT (fftOrder),
      window (fftSize, juce::dsp::WindowingFunction<float>::hann)
{
    fifo.assign (fftSize, 0.0f);
    fftData.assign (2 * fftSize, 0.0f);
    chromaVector.assign (12, 0.0f);
}

ChromaExtractor::~ChromaExtractor()
{
}

void ChromaExtractor::prepare (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    std::fill (chromaVector.begin(), chromaVector.end(), 0.0f);
    std::fill (fifo.begin(), fifo.end(), 0.0f);
    fifoIndex = 0;
}

void ChromaExtractor::process (const juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() == 0) return;

    const float* channelData = buffer.getReadPointer (0);
    int numSamples = buffer.getNumSamples();

    for (int i = 0; i < numSamples; ++i)
    {
        pushNextSampleIntoFifo (channelData[i]);
    }
}

void ChromaExtractor::pushNextSampleIntoFifo (float sample)
{
    if (fifoIndex == fftSize)
    {
        std::fill (fftData.begin(), fftData.end(), 0.0f);
        std::copy (fifo.begin(), fifo.end(), fftData.begin());
        performFFT();
        fifoIndex = 0;
    }

    fifo[fifoIndex++] = sample;
}

void ChromaExtractor::performFFT()
{
    window.multiplyWithWindowingTable (fftData.data(), fftSize);
    forwardFFT.performFrequencyOnlyForwardTransform (fftData.data());

    for (auto& val : chromaVector)
        val *= 0.85f;

    int numBins = fftSize / 2;
    for (int bin = 1; bin < numBins; ++bin)
    {
        float frequency = static_cast<float> (bin * currentSampleRate / fftSize);
        if (frequency < 50.0f || frequency > 4000.0f) continue;

        float midiNote = 12.0f * std::log2 (frequency / 440.0f) + 69.0f;
        int noteIndex = static_cast<int> (std::round (midiNote)) % 12;
        if (noteIndex < 0) noteIndex += 12;

        float magnitude = fftData[bin];
        chromaVector[noteIndex] += magnitude;
    }
}

const std::vector<float>& ChromaExtractor::getChromaVector() const
{
    return chromaVector;
}
