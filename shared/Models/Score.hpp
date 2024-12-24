#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "shared/Models/Player.hpp"

#include <string>
using namespace std;

struct ScoreImprovement
{
    int score = 0;
    float accuracy;
    int rank;
    float pp;

    int totalRank;
    float totalPp;

    ScoreImprovement();
    ScoreImprovement(rapidjson::Value const& document);
};

struct Score
{
    int id;
    int baseScore;
    int modifiedScore;
    float accuracy;
    string playerId;
    float pp;
    float weight;
    float multiplier;
    int rank;
    int countryRank;
    string modifiers;
    int badCuts;
    int missedNotes;
    int maxCombo;
    // int wallsHit;
    bool fullCombo;
    bool hasReplay;
    int headsetId;
    string headsetName;
    string leftControllerName;
    string rightControllerName;
    string timeset;
    int maxScore;

    Player player;
    int leaderboardId;
    //useless maybe
    // ScoreImprovement scoreImprovement;
    Score();
    Score(rapidjson::Value const& document);
    void setMaxScore(int maxScore);
    void setLeaderboardId(int leaderboardId);
};
