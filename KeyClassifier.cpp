#include "KeyClassifier.h"
#include <numeric>
#include <cmath>
#include <algorithm>

KeyClassifier::KeyClassifier()
{
    majorProfile = { 6.35f, 2.23f, 3.48f, 2.33f, 4.38f, 4.09f, 2.52f, 5.19f, 2.39f, 3.66f, 2.29f, 2.88f };
    minorProfile = { 6.33f, 2.68f, 3.52f, 5.38f, 2.60f, 3.53f, 2.54f, 4.75f, 3.98f, 2.69f, 3.34f, 3.17f };
}

KeyClassifier::~KeyClassifier()
{
}

float KeyClassifier::calculateCorrelation (const std::vector<float>& v1, const std::vector<float>& v2)
{
    if (v1.size() != v2.size() || v1.empty()) return 0.0f;

    float sum1 = std::accumulate (v1.begin(), v1.end(), 0.0f);
    float sum2 = std::accumulate (v2.begin(), v2.end(), 0.0f);

    float mean1 = sum1 / v1.size();
    float mean2 = sum2 / v2.size();

    float num = 0.0f, den1 = 0.0f, den2 = 0.0f;

    for (size_t i = 0; i < v1.size(); ++i)
    {
        float diff1 = v1[i] - mean1;
        float diff2 = v2[i] - mean2;

        num += diff1 * diff2;
        den1 += diff1 * diff1;
        den2 += diff2 * diff2;
    }

    if (den1 == 0.0f || den2 == 0.0f) return 0.0f;

    return num / std::sqrt (den1 * den2);
}

std::string KeyClassifier::detectKey (const std::vector<float>& chromaVector)
{
    const std::string noteNames[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

    float maxCorrelation = -100.0f;
    std::string detectedKey = "A analisar...";

    for (int root = 0; root < 12; ++root)
    {
        std::vector<float> shiftedChroma (12);
        for (int i = 0; i < 12; ++i)
        {
            shiftedChroma[i] = chromaVector[(i + root) % 12];
        }

        float corrMajor = calculateCorrelation (shiftedChroma, majorProfile);
        if (corrMajor > maxCorrelation)
        {
            maxCorrelation = corrMajor;
            detectedKey = noteNames[root] + " Major";
        }

        float corrMinor = calculateCorrelation (shiftedChroma, minorProfile);
        if (corrMinor > maxCorrelation)
        {
            maxCorrelation = corrMinor;
            detectedKey = noteNames[root] + " Minor";
        }
    }

    return detectedKey;
}
