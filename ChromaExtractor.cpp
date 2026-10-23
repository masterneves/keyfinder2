#include "ChromaExtractor.h"

#include <algorithm>
#include <cmath>

namespace keydetect
{
    ChromaExtractor::ChromaExtractor()
        : fft (fftOrder),
          window ((size_t) fftSize, juce::dsp::WindowingFunction<float>::hann, false),
          inputBuffer ((size_t) fftSize, 0.0f),
          fftData ((size_t) fftSize * 2, 0.0f)
    {
        prepare (44100.0, 0.0);
    }

    void ChromaExtractor::prepare (double newSampleRate, double memorySeconds)
    {
        sampleRate = newSampleRate > 0.0 ? newSampleRate : 44100.0;
        binHz      = (float) (sampleRate / (double) fftSize);

        // Intervalo de bins usado para o chroma (garante k-1 e k+1 válidos para o pico)
        kMin = std::max (1, (int) std::ceil (minFreqHz / binHz));
        kMax = std::min (fftSize / 2 - 1, (int) std::floor (maxFreqHz / binHz));
        if (kMax <= kMin)
            kMax = kMin + 1;

        // Decaimento por hop: exp(-T_hop / T_memória)
        const double hopSeconds = (double) hopSize / sampleRate;
        decay = memorySeconds > 0.0 ? (float) std::exp (-hopSeconds / memorySeconds) : 1.0f;

        resetInternal();
    }

    void ChromaExtractor::resetInternal() noexcept
    {
        std::fill (inputBuffer.begin(), inputBuffer.end(), 0.0f);
        writePos = 0;
        accumulator.fill (0.0f);
        for (auto& v : shared)
            v.store (0.0f, std::memory_order_relaxed);
        framesAnalysed.store (0);
    }

    void ChromaExtractor::pushSamples (const float* mono, int numSamples) noexcept
    {
        if (resetRequested.exchange (false))
            resetInternal();

        for (int i = 0; i < numSamples; ++i)
        {
            inputBuffer[(size_t) writePos++] = mono[i];

            if (writePos == fftSize)
            {
                processFrame();

                // Overlap de 50 %: a segunda metade passa a ser a primeira
                std::copy (inputBuffer.begin() + hopSize, inputBuffer.end(), inputBuffer.begin());
                writePos = hopSize;
            }
        }
    }

    void ChromaExtractor::processFrame() noexcept
    {
        // 1) Gate de silêncio
        double energy = 0.0;
        for (const float s : inputBuffer)
            energy += (double) s * (double) s;

        if (std::sqrt (energy / (double) fftSize) < (double) silenceRms)
            return;

        // 2) Janela de Hann + FFT (zero-padding na segunda metade do buffer exigido pela JUCE)
        std::copy (inputBuffer.begin(), inputBuffer.end(), fftData.begin());
        std::fill (fftData.begin() + fftSize, fftData.end(), 0.0f);

        window.multiplyWithWindowingTable (fftData.data(), (size_t) fftSize);
        fft.performFrequencyOnlyForwardTransform (fftData.data());   // bins [0, fftSize/2] = magnitude

        // 3) Máximo do espectro na banda de interesse
        float maxMag = 0.0f;
        for (int k = kMin; k <= kMax; ++k)
            maxMag = std::max (maxMag, fftData[(size_t) k]);

        if (maxMag <= 1.0e-9f)
            return;

        // 4) Picos espectrais -> classe de nota
        std::array<float, 12> frame {};
        const float threshold = maxMag * peakThreshold;

        for (int k = kMin; k <= kMax; ++k)
        {
            const float m = fftData[(size_t) k];
            const float a = fftData[(size_t) k - 1];
            const float c = fftData[(size_t) k + 1];

            if (m < threshold || m <= a || m < c)
                continue;                                   // não é máximo local

            // Interpolação parabólica para refinar a frequência do pico
            const float denom = a - 2.0f * m + c;
            const float delta = std::abs (denom) > 1.0e-12f ? 0.5f * (a - c) / denom : 0.0f;
            const float freq  = ((float) k + delta) * binHz;

            if (freq <= 0.0f)
                continue;

            // f -> nota MIDI (A4 = 440 Hz = 69); classe de nota: MIDI mod 12 (C = 0)
            const float midi = 69.0f + 12.0f * std::log2 (freq / 440.0f);
            const int   pc   = ((int) std::lround (midi) % 12 + 12) % 12;

            // Peso: raiz da amplitude relativa (comprime a dinâmica entre parciais)
            frame[(size_t) pc] += std::sqrt (m / maxMag);
        }

        float sum = 0.0f;
        for (const float v : frame)
            sum += v;

        if (sum <= 1.0e-9f)
            return;

        // 5) Acumulação: cada frame contribui com o mesmo peso (normalizado), com decaimento opcional
        for (size_t i = 0; i < 12; ++i)
        {
            accumulator[i] = accumulator[i] * decay + frame[i] / sum;
            shared[i].store (accumulator[i], std::memory_order_relaxed);
        }

        framesAnalysed.fetch_add (1);
    }

    ChromaVector ChromaExtractor::getChroma() const noexcept
    {
        ChromaVector out {};
        float maxValue = 0.0f;

        for (size_t i = 0; i < 12; ++i)
        {
            out[i] = shared[i].load (std::memory_order_relaxed);
            maxValue = std::max (maxValue, out[i]);
        }

        if (maxValue > 1.0e-9f)
            for (auto& v : out)
                v /= maxValue;

        return out;
    }
}
