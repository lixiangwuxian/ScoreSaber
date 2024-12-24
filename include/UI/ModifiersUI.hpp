#pragma once

#include <unordered_map>
#include <string>

using namespace std;

namespace ModifiersUI {
    extern unordered_map<string, float> songModifiers;
    extern unordered_map<string, float> songModifierRatings;

    void setup();
    float refreshMultiplierAndMaxRank();
    float refreshAllModifiers();
    void SetModifiersActive(bool active);
    void ResetModifiersUI();
    bool ModifiersAvailable();
}