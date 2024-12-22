#include "shared/Models/Score.hpp"

Score::Score() {}

Score::Score(rapidjson::Value const& document) {
    auto const& playerObject = document["leaderboardPlayerInfo"];
    player = Player(playerObject);

    playerId = player.id;

    id = document.HasMember("id") && !document["id"].IsNull() ? document["id"].GetInt() : 0;
    rank = document.HasMember("rank") && !document["rank"].IsNull() ? document["rank"].GetInt() : 0;
    baseScore = document.HasMember("baseScore") && !document["baseScore"].IsNull() ? document["baseScore"].GetInt() : 0;
    modifiedScore = document.HasMember("modifiedScore") && !document["modifiedScore"].IsNull() ? document["modifiedScore"].GetInt() : 0;
    
    pp = document.HasMember("pp") && !document["pp"].IsNull() ? document["pp"].GetDouble() : 0.0;
    weight = document.HasMember("weight") && !document["weight"].IsNull() ? document["weight"].GetDouble() : 0.0;
    
    modifiers = document.HasMember("modifiers") && !document["modifiers"].IsNull() ? document["modifiers"].GetString() : "";
    multiplier = document.HasMember("multiplier") && !document["multiplier"].IsNull() ? document["multiplier"].GetDouble() : 0.0;
    
    badCuts = document.HasMember("badCuts") && !document["badCuts"].IsNull() ? document["badCuts"].GetInt() : 0;
    missedNotes = document.HasMember("missedNotes") && !document["missedNotes"].IsNull() ? document["missedNotes"].GetInt() : 0;
    maxCombo = document.HasMember("maxCombo") && !document["maxCombo"].IsNull() ? document["maxCombo"].GetInt() : 0;
    
    fullCombo = document.HasMember("fullCombo") && !document["fullCombo"].IsNull() ? document["fullCombo"].GetBool() : false;
    
    headsetId = document.HasMember("hmd") && !document["hmd"].IsNull() ? document["hmd"].GetInt() : 0;
    headsetName = document.HasMember("deviceHmd") && !document["deviceHmd"].IsNull() && document["deviceHmd"].IsString() ? document["deviceHmd"].GetString() : "";
    leftControllerName = document.HasMember("deviceControllerLeft") && !document["deviceControllerLeft"].IsNull() && document["deviceControllerLeft"].IsString() ? document["deviceControllerLeft"].GetString() : "";
    rightControllerName = document.HasMember("deviceControllerRight") && !document["deviceControllerRight"].IsNull() && document["deviceControllerRight"].IsString() ? document["deviceControllerRight"].GetString() : "";
    
    hasReplay = document.HasMember("hasReplay") && !document["hasReplay"].IsNull() ? document["hasReplay"].GetBool() : false;
    timeset = document.HasMember("timeSet") && !document["timeSet"].IsNull() ? document["timeSet"].GetString() : "";
}

ScoreImprovement::ScoreImprovement() {}

ScoreImprovement::ScoreImprovement(rapidjson::Value const& document) {
    score = document["score"].GetInt();
    accuracy = document["accuracy"].GetFloat();
    rank = document["rank"].GetInt();
    pp = document["pp"].GetFloat();

    totalRank = document["totalRank"].GetInt();
    totalPp = document["totalPp"].GetFloat();
}