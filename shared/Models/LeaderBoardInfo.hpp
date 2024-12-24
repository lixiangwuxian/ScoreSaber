#pragma once
#include <string>
#include "Difficulty.hpp"
class LeaderBoardInfo {
public:
    int id;
    std::string songHash;
    std::string songName;
    std::string songSubName;
    std::string songAuthorName;
    std::string levelAuthorName;
    
    Difficulty difficulty;

    int maxScore;
    std::string createdDate;
    std::string rankedDate;
    std::string qualifiedDate;
    std::string lovedDate;
    bool ranked;
    bool qualified;
    bool loved;
    float maxPP;
    float stars;
    int plays;
    int dailyPlays;
    bool positiveModifiers;
    std::string coverImage;

    LeaderBoardInfo(rapidjson::Value const& document);
    LeaderBoardInfo()=default;
};