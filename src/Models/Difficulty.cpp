#include "shared/Models/Difficulty.hpp"

Difficulty::Difficulty(rapidjson::Value const& document) {
    leaderboardId = document["leaderboardId"].GetInt();
    difficulty = document["difficulty"].GetInt();
    gameMode = document["gameMode"].GetString();
    difficultyRaw = document["difficultyRaw"].GetString();
}

Difficulty::Difficulty(int leaderboardId, int difficulty, string gameMode, string difficultyRaw) {
    this->leaderboardId = leaderboardId;
    this->difficulty = difficulty;
    this->gameMode = gameMode;
    this->difficultyRaw = difficultyRaw;
}