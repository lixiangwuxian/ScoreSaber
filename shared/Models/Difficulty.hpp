#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "shared/Models/Clan.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
using namespace std;

struct Difficulty 
{
    //ss
    int leaderboardId;
    int difficulty;
    std::string gameMode;
    std::string difficultyRaw;
    //

    Difficulty(rapidjson::Value const& document);
    Difficulty(int leaderboardId, int difficulty, string gameMode, string difficultyRaw);
    Difficulty() = default;
};