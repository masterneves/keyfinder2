#pragma once

#include "ChromaExtractor.h"

#include <vector>

namespace keydetect
{
    enum class ScaleType
    {
        Major,
        NaturalMinor,
        HarmonicMinor,
        PentatonicMajor,
        PentatonicMinor,
        Dorian,
        Mixolydian,
        Lydian,
        Phrygian
    };

    struct ScaleCandidate
    {
        int       root  = 0;                    // 0 = C ... 11 = B
        ScaleType type  = ScaleType::Major;
        float     score = 0.0f;                 // pontuação híbrida (aprox. correlação, -1..1)
    };

    struct KeyResult
    {
        bool  valid = false;
        float confidence = 0.0f;                // 0..1
        std::vector<ScaleCandidate> ranking;    // melhores candidatos, por ordem decrescente

        const ScaleCandidate& best() const { return ranking.front(); }
    };

    const char* getNoteName (int pitchClass) noexcept;
    const char* getScaleName (ScaleType type) noexcept;
    bool isNoteInScale (int root, ScaleType type, int pitchClass) noexcept;

    /** Classifica o vetor chroma.
        @param includeModes  se false, só considera Maior, Menor Natural/Harmónica e Pentatónicas. */
    KeyResult classify (const ChromaVector& chroma, bool includeModes);
}
