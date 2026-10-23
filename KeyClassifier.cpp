#include "KeyClassifier.h"

KeyClassifier::KeyClassifier()
{
}

KeyClassifier::~KeyClassifier()
{
}

std::string KeyClassifier::detectKey (const std::vector<float>& chromaVector)
{
    return "C Major";
}
