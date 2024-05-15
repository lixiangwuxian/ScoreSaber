#include "Data/Private/ScoreSaberUploadData.hpp"

using namespace ScoreSaber::Data::Private;

ScoreSaberUploadData::ScoreSaberUploadData()
{
}

ScoreSaberUploadData::~ScoreSaberUploadData()
{
}

ScoreSaberUploadData::ScoreSaberUploadData(string playerName, string playerId, int score, string leaderboardId,
                                           string songName, string songSubName, string levelAuthorName, string songAuthorName, int bpm, int difficulty,
                                           string infoHash, vector<string> modifiers, string gameMode, int badCutsCount, int missedCount, int maxCombo,
                                           bool fullCombo, string deviceHmdIdentifier, string deviceControllerLeftIdentifier, string deviceControllerRightIdentifier)
    : playerName(playerName), playerId(playerId), score(score), leaderboardId(leaderboardId), songName(songName), songSubName(songSubName),
      levelAuthorName(levelAuthorName), songAuthorName(songAuthorName), bpm(bpm), difficulty(difficulty), infoHash(infoHash), modifiers(modifiers),
      gameMode(gameMode), badCutsCount(badCutsCount), missedCount(missedCount), maxCombo(maxCombo), fullCombo(fullCombo), deviceHmdIdentifier(deviceHmdIdentifier),
      deviceControllerLeftIdentifier(deviceControllerLeftIdentifier), deviceControllerRightIdentifier(deviceControllerRightIdentifier)
{
}