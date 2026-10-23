#pragma once

#include <vector>
#include <string>

class KeyClassifier
{
public:
    KeyClassifier();
    ~KeyClassifier();

    std::string detectKey (const std::vector<float>& chromaVector);

private:
    std::vector<float> majorProfile;
    std::vector<float> minorProfile;

    float calculateCorrelation (const std::vector<float>& v1, const std::vector<float>& v2);
};
