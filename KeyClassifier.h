#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>

class KeyClassifier
{
public:
    KeyClassifier();
    ~KeyClassifier();

    std::string detectKey (const std::vector<float>& chromaVector);
};
