#include "ChromaExtractor.h"

ChromaExtractor::ChromaExtractor()
{
}

ChromaExtractor::~ChromaExtractor()
{
}

void ChromaExtractor::prepare (double sampleRate, int samplesPerBlock)
{
    chromaVector.assign (12, 0.0f);
}

void ChromaExtractor::process (const juce::AudioBuffer<float>& buffer)
{
    // Implementação básica de extração de chroma
}

const std::vector<float>& ChromaExtractor::getChromaVector() const
{
    return chromaVector;
}
