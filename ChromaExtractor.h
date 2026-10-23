#pragma once

#include <juce_dsp/juce_dsp.h>

#include <array>
#include <atomic>
#include <vector>

namespace keydetect
{
    /** 12 classes de notas: C, C#, D, D#, E, F, F#, G, G#, A, A#, B. */
    using ChromaVector = std::array<float, 12>;

    /**
     * Extrai um vetor chroma (Pitch Class Profile) a partir de um fluxo de amostras mono.
     *
     * Pipeline (STFT):
     *   amostras -> janela de Hann (16384 pts, 50 % overlap) -> FFT -> espectro de magnitude
     *   -> deteção de picos espectrais (80 Hz - 2 kHz) com interpolação parabólica
     *   -> frequência -> nota MIDI -> classe de nota (0..11) -> acumulação ponderada.
     *
     * Notas de utilização:
     *  - pushSamples() é seguro para a thread de áudio (sem alocações, sem locks).
     *  - getChroma() pode ser chamado de qualquer thread (lê valores atómicos).
     *  - prepare() NÃO deve ser chamado em simultâneo com pushSamples().
     */
    class ChromaExtractor
    {
    public:
        static constexpr int fftOrder = 14;                 // 16384 pontos (~2.7 Hz/bin a 44.1 kHz)
        static constexpr int fftSize  = 1 << fftOrder;
        static constexpr int hopSize  = fftSize / 2;

        ChromaExtractor();

        /** Define a taxa de amostragem e a "memória" do perfil.
            memorySeconds <= 0  -> sem esquecimento (análise de ficheiro completo).
            memorySeconds  > 0  -> decaimento exponencial (modo tempo real). */
        void prepare (double newSampleRate, double memorySeconds);

        /** Pede o reset do estado; é executado com segurança na próxima chamada a pushSamples(). */
        void requestReset() noexcept { resetRequested.store (true); }

        /** Alimenta o analisador com amostras mono. */
        void pushSamples (const float* mono, int numSamples) noexcept;

        /** Devolve o chroma acumulado, normalizado para máximo = 1 (tudo zero se não há dados). */
        ChromaVector getChroma() const noexcept;

        /** Número de frames de FFT não silenciosos já analisados. */
        int getFramesAnalysed() const noexcept { return framesAnalysed.load(); }

    private:
        void resetInternal() noexcept;
        void processFrame() noexcept;

        static constexpr float minFreqHz     = 80.0f;
        static constexpr float maxFreqHz     = 2000.0f;
        static constexpr float silenceRms    = 0.001f;   // ~ -60 dBFS: frames abaixo disto são ignorados
        static constexpr float peakThreshold = 0.03f;    // picos < 3 % do máximo do frame são ruído

        juce::dsp::FFT fft;
        juce::dsp::WindowingFunction<float> window;

        std::vector<float> inputBuffer;   // fftSize
        std::vector<float> fftData;       // 2 * fftSize (exigido pelo juce::dsp::FFT)
        int writePos = 0;

        double sampleRate = 44100.0;
        float  binHz = 0.0f;
        int    kMin = 1, kMax = 1;
        float  decay = 1.0f;

        std::array<float, 12> accumulator {};                 // só usado pela thread que faz push
        std::array<std::atomic<float>, 12> shared {};         // cópia para leitura pela GUI
        std::atomic<int>  framesAnalysed { 0 };
        std::atomic<bool> resetRequested { false };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChromaExtractor)
    };
}
