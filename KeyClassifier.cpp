#include "KeyClassifier.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace keydetect
{
    namespace
    {
        // Perfis de tonalidade de Krumhansl-Kessler (graus 0..11 a partir da tónica)
        constexpr std::array<float, 12> kkMajor { 6.35f, 2.23f, 3.48f, 2.33f, 4.38f, 4.09f,
                                                  2.52f, 5.19f, 2.39f, 3.66f, 2.29f, 2.88f };
        constexpr std::array<float, 12> kkMinor { 6.33f, 2.68f, 3.52f, 5.38f, 2.60f, 3.53f,
                                                  2.54f, 4.75f, 3.98f, 2.69f, 3.34f, 3.17f };

        constexpr const char* noteNames[12] = { "C", "C#", "D", "D#", "E", "F",
                                                "F#", "G", "G#", "A", "A#", "B" };

        struct ScaleDef
        {
            ScaleType type;
            const char* name;
            std::vector<int> intervals;     // semitons a partir da tónica
            bool minorFamily;               // usa o perfil K-K menor (senão, o maior)
            bool isMode;                    // modo (Dorian, ...): só entra se includeModes = true
            float bias;                     // prior: favorece escalas mais comuns
            std::array<float, 12> templ;    // template ponderado (tónica/quinta/terça em destaque)
        };

        /** Template: membros da escala = 0.55; tónica = 1.0; quinta = 0.8; terça = 0.7; resto = 0. */
        std::array<float, 12> buildTemplate (const std::vector<int>& intervals)
        {
            std::array<float, 12> t {};
            for (const int d : intervals)
                t[(size_t) d] = 0.55f;

            t[0] = 1.0f;
            if (t[7] > 0.0f) t[7] = 0.8f;
            if (t[3] > 0.0f) t[3] = 0.7f;
            if (t[4] > 0.0f) t[4] = 0.7f;
            return t;
        }

        const std::vector<ScaleDef>& scaleDefs()
        {
            static const std::vector<ScaleDef> defs = []
            {
                std::vector<ScaleDef> v;
                auto add = [&v] (ScaleType type, const char* name, std::vector<int> intervals,
                                 bool minor, bool mode, float bias)
                {
                    ScaleDef d { type, name, std::move (intervals), minor, mode, bias, {} };
                    d.templ = buildTemplate (d.intervals);
                    v.push_back (std::move (d));
                };

                //   tipo                           nome                 intervalos                   menor  modo   bias
                add (ScaleType::Major,            "Major",             { 0, 2, 4, 5, 7, 9, 11 },    false, false,  0.00f);
                add (ScaleType::NaturalMinor,     "Natural Minor",     { 0, 2, 3, 5, 7, 8, 10 },    true,  false,  0.00f);
                add (ScaleType::HarmonicMinor,    "Harmonic Minor",    { 0, 2, 3, 5, 7, 8, 11 },    true,  false, -0.03f);
                add (ScaleType::PentatonicMajor,  "Pentatonic Major",  { 0, 2, 4, 7, 9 },           false, false, -0.02f);
                add (ScaleType::PentatonicMinor,  "Pentatonic Minor",  { 0, 3, 5, 7, 10 },          true,  false, -0.02f);
                add (ScaleType::Dorian,           "Dorian",            { 0, 2, 3, 5, 7, 9, 10 },    true,  true,  -0.04f);
                add (ScaleType::Mixolydian,       "Mixolydian",        { 0, 2, 4, 5, 7, 9, 10 },    false, true,  -0.04f);
                add (ScaleType::Lydian,           "Lydian",            { 0, 2, 4, 6, 7, 9, 11 },    false, true,  -0.04f);
                add (ScaleType::Phrygian,         "Phrygian",          { 0, 1, 3, 5, 7, 8, 10 },    true,  true,  -0.04f);
                return v;
            }();
            return defs;
        }

        const ScaleDef* findDef (ScaleType type) noexcept
        {
            for (const auto& d : scaleDefs())
                if (d.type == type)
                    return &d;
            return nullptr;
        }

        /** Roda um perfil (definido relativamente à tónica) para a tónica `root`. */
        std::array<float, 12> rotateProfile (const std::array<float, 12>& profile, int root) noexcept
        {
            std::array<float, 12> r {};
            for (int i = 0; i < 12; ++i)
                r[(size_t) ((i + root) % 12)] = profile[(size_t) i];
            return r;
        }

        /** Coeficiente de correlação de Pearson entre dois vetores de 12 elementos. */
        float pearson (const std::array<float, 12>& a, const std::array<float, 12>& b) noexcept
        {
            float meanA = 0.0f, meanB = 0.0f;
            for (size_t i = 0; i < 12; ++i) { meanA += a[i]; meanB += b[i]; }
            meanA /= 12.0f;
            meanB /= 12.0f;

            float num = 0.0f, da = 0.0f, db = 0.0f;
            for (size_t i = 0; i < 12; ++i)
            {
                const float x = a[i] - meanA;
                const float y = b[i] - meanB;
                num += x * y;
                da  += x * x;
                db  += y * y;
            }

            if (da < 1.0e-12f || db < 1.0e-12f)
                return 0.0f;

            return num / std::sqrt (da * db);
        }

        unsigned pitchMask (int root, const std::vector<int>& intervals) noexcept
        {
            unsigned mask = 0;
            for (const int d : intervals)
                mask |= 1u << ((root + d) % 12);
            return mask;
        }

        float clamp01 (float x) noexcept { return std::min (1.0f, std::max (0.0f, x)); }
    }

    const char* getNoteName (int pitchClass) noexcept
    {
        return noteNames[((pitchClass % 12) + 12) % 12];
    }

    const char* getScaleName (ScaleType type) noexcept
    {
        if (const auto* d = findDef (type))
            return d->name;
        return "?";
    }

    bool isNoteInScale (int root, ScaleType type, int pitchClass) noexcept
    {
        const auto* d = findDef (type);
        if (d == nullptr)
            return false;

        const int rel = (((pitchClass - root) % 12) + 12) % 12;
        return std::find (d->intervals.begin(), d->intervals.end(), rel) != d->intervals.end();
    }

    KeyResult classify (const ChromaVector& chroma, bool includeModes)
    {
        KeyResult result;

        const float total = std::accumulate (chroma.begin(), chroma.end(), 0.0f);
        if (total < 1.0e-6f)
            return result;                                   // sem dados -> resultado inválido

        struct Scored { ScaleCandidate cand; unsigned mask; };
        std::vector<Scored> all;
        all.reserve (12 * scaleDefs().size());

        // Pontuação híbrida: 50 % correlação com o perfil K-K (localiza bem a tónica e o
        // modo maior/menor) + 50 % correlação com o template binário da escala (distingue
        // escalas com notas diferentes, p. ex. Pentatónica vs Diatónica, Natural vs Harmónica).
        for (int root = 0; root < 12; ++root)
        {
            for (const auto& def : scaleDefs())
            {
                if (def.isMode && ! includeModes)
                    continue;

                const float kk = pearson (chroma, rotateProfile (def.minorFamily ? kkMinor : kkMajor, root));
                const float tp = pearson (chroma, rotateProfile (def.templ, root));

                all.push_back ({ { root, def.type, 0.5f * (kk + tp) + def.bias },
                                 pitchMask (root, def.intervals) });
            }
        }

        if (all.empty())
            return result;

        std::sort (all.begin(), all.end(),
                   [] (const Scored& a, const Scored& b) { return a.cand.score > b.cand.score; });

        // Segundo melhor com conjunto de notas DIFERENTE (relativas como C Maior / A Menor
        // partilham notas e não devem penalizar a confiança).
        const float r1 = all.front().cand.score;
        float r2 = -1.0f;
        for (size_t i = 1; i < all.size(); ++i)
        {
            if (all[i].mask != all.front().mask)
            {
                r2 = all[i].cand.score;
                break;
            }
        }

        // Confiança heurística: 60 % força absoluta + 40 % margem sobre o rival.
        const float absolute = clamp01 ((r1 - 0.35f) / 0.55f);
        const float margin   = clamp01 ((r1 - r2) / 0.10f);
        result.confidence    = clamp01 (0.6f * absolute + 0.4f * margin);

        const size_t keep = std::min<size_t> (5, all.size());
        for (size_t i = 0; i < keep; ++i)
            result.ranking.push_back (all[i].cand);

        result.valid = true;
        return result;
    }
}
