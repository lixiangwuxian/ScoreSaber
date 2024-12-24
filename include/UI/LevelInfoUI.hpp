#pragma once

#include "shared/Models/LeaderBoardInfo.hpp"

namespace LevelInfoUI {
    void setup();
    void reset();
    void SetLevelInfoActive(bool active);

    void resetStars();
    // void addVoteToCurrentLevel(bool rankable, int type);

    void setLabels(LeaderBoardInfo leaderBoardInfo);
    void refreshRatingLabels();
    void setRatingLabels(float rating);
}