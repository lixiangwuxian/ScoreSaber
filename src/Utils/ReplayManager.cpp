#include "include/Utils/FileManager.hpp"
#include "include/Utils/ReplayManager.hpp"
#include "include/UI/ModifiersUI.hpp"
#include "include/Utils/WebUtils.hpp"
#include "include/Utils/StringUtils.hpp"
#include "include/Utils/ModConfig.hpp"
#include "include/API/PlayerController.hpp"
#include "include/main.hpp"

#include <sys/stat.h>
#include <stdio.h>

string ReplayManager::lastReplayFilename = "";
PlayEndData ReplayManager::lastReplayStatus = PlayEndData(LevelEndType::Fail, 0);

void ReplayManager::ProcessReplay(Replay const &replay, PlayEndData status, bool skipUpload, function<void(ReplayUploadStatus, string, float,
                                                                                  int)> const &finished) {
    if (status.GetEndType() == LevelEndType::Unknown) {
        return;
    }
    
    string filename = FileManager::ToFilePath(replay);
    lastReplayFilename = filename;

    FileManager::WriteReplay(replay);
    ScoreSaberLogger.info("{}",("Replay saved " + filename).c_str());

    if(!UploadEnabled()) {
        finished(ReplayUploadStatus::finished, "<color=#BB2020ff>Upload disabled. But replay was saved.</color>", 0, -1);
        return;
    }    
    
    if (replay.info.failTime > 0.001 || replay.info.speed > 0.001) {
        finished(ReplayUploadStatus::finished, "<color=#BB2020ff>Failed attempt was saved!</color>", 0, -1);
    }
    if(skipUpload)
        return;
    if(PlayerController::currentPlayer != std::nullopt)
        TryPostReplay(filename, status, 0, finished);
}

void ReplayManager::TryPostReplay(string name, PlayEndData status, int tryIndex, function<void(ReplayUploadStatus, string, float,
                                                                                int)> const &finished) {
    struct stat file_info;
    stat(name.data(), &file_info);

    lastReplayStatus = status;
    bool runCallback = status.GetEndType() == LevelEndType::Clear;
    if (tryIndex == 0) {
        ScoreSaberLogger.info("{}",("Started posting " + to_string(file_info.st_size)).c_str());
        if (runCallback) {
            finished(ReplayUploadStatus::inProgress, "<color=#b103fcff>Posting replay...", 0, 0);
        }
    }
    FILE *replayFile = fopen(name.data(), "rb");
    chrono::steady_clock::time_point replayPostStart = chrono::steady_clock::now();
    //todo debug
    finished(ReplayUploadStatus::finished, "<color=#20BB20ff>Not implemented yet So not posted lol</color>", 100, 200);
    return;
    //
    WebUtils::PostFileAsync(WebUtils::API_URL + "replayoculus" + status.ToQueryString(), replayFile, (long)file_info.st_size, [name, tryIndex, finished, replayFile, replayPostStart, runCallback, status](long statusCode, string result, string headers) {
        fclose(replayFile);
        if ((statusCode >= 450 || statusCode < 200) && tryIndex < 2) {
            ScoreSaberLogger.info("{}", ("Retrying posting replay after " + to_string(statusCode) + " #" + to_string(tryIndex) + " " + std::string(result)).c_str());
            if (statusCode == 100) {
                result = "Timed out";
            }
            if (runCallback) {
                finished(ReplayUploadStatus::inProgress, "<color=#ffff00ff>Retrying posting replay after " + to_string(statusCode) + " try #" + to_string(tryIndex) + " " + std::string(result) + "</color>", 0, statusCode);
            }
            TryPostReplay(name, status, tryIndex + 1, finished);
        } else if (statusCode == 200) {
            auto duration = chrono::duration_cast<std::chrono::milliseconds>(chrono::steady_clock::now() - replayPostStart).count();
            ScoreSaberLogger.info("{}", ("Replay was posted! It took: " + to_string((int)duration) + "msec. \n").c_str());
            if (runCallback) {
                finished(ReplayUploadStatus::finished, "<color=#20BB20ff>Replay was posted!</color>", 100, statusCode);
            }
            if (!getModConfig().SaveLocalReplays.GetValue()) {
                remove(name.data());
            }
        } else {
            if (statusCode == 100 || statusCode == 0) {
                statusCode = 100;
                result = "Timed out";
            }
            ScoreSaberLogger.error("{}", ("Replay was not posted! " + to_string(statusCode) + result).c_str());
            if (runCallback) {
                finished(ReplayUploadStatus::error, std::string("<color=#BB2020ff>Replay was not posted. " + result), 0, statusCode);
            }
        }
    }, [finished, runCallback](float percent) {
        if (runCallback) {
            finished(ReplayUploadStatus::inProgress, "<color=#b103fcff>Posting replay: " + to_string_wprecision(percent, 2) + "%", percent, 0);
        }
    });
}

void ReplayManager::RetryPosting(const std::function<void(ReplayUploadStatus, std::string, float, int)>& finished) {
    TryPostReplay(lastReplayFilename, lastReplayStatus, 0, finished);
};

int ReplayManager::GetLocalScore(string filename) {
    struct stat buffer;
    if ((stat (filename.data(), &buffer) == 0)) {
        auto info = FileManager::ReadInfo(filename);
        if (info != std::nullopt) {
            return (int)((float)info->score * GetTotalMultiplier(info->modifiers)); 
        }
    }

    return 0;
} 

float ReplayManager::GetTotalMultiplier(string modifiers) {
    float multiplier = 1;

    auto modifierValues = ModifiersUI::songModifiers;
    if(!modifierValues.empty()) {
        for (auto& [key, value] : modifierValues){
            if (modifiers.find(key) != string::npos) {
                multiplier += value;
            }
        }
    }

    return multiplier;
}


//todo add upload refer this
// void Five(Replay const &replay, PlayEndData status, bool skipUpload, function<void(ReplayUploadStatus, string, float,
//                                                                                   int)> const &finished)
//     {

//         // if (GetEnv("disable_ss_upload") == "1")
//         if (!UploadEnabled()||skipUpload)
//         {
//             return;
//         }

//         // if (ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying) 
//         // {
//         //     return;
//         // }

//         // PracticeViewController* practiceViewController = QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<PracticeViewController*>());
//         // if (practiceViewController->get_isInViewControllerHierarchy())
//         // {
//         //     ReplayService::WriteSerializedReplay();
//         //     return;
//         // }

//         if (gameMode == "Solo" || gameMode == "Multiplayer")
//         {
//             auto level = difficultyBeatmap->get_level()->i_IPreviewBeatmapLevel();
//             INFO("Starting upload process for %s:%s", ((string)(level->get_levelID())).c_str(), ((string)(level->get_songName())).c_str());
//             if (practicing) {
//                 ReplayService::WriteSerializedReplay();
//                 return;
//             }
//             if (status.GetEndType() != LevelEndType::Unknown) {
//                 if (status.GetEndType() == LevelEndType::Restart) {
//                     INFO("Level was restarted before it was finished, don't write replay");
//                 } else {
//                     ReplayService::WriteSerializedReplay();
//                 }
//                 return;
//             }
//             if (status.GetEndType() != LevelEndType::Clear) {
//                 ReplayService::WriteSerializedReplay();
//                 return;
//             }

//             ReplayService::WriteSerializedReplay();
//             // Continue to upload phase
//             Six(difficultyBeatmap, levelCompletionResults);
//         }
//     }

//     void Six(GlobalNamespace::IDifficultyBeatmap* beatmap, GlobalNamespace::LevelCompletionResults* levelCompletionResults)
//     {

//         std::string encryptedPacket = CreateScorePacket(beatmap, levelCompletionResults->multipliedScore, levelCompletionResults->modifiedScore,
//                                                         levelCompletionResults->fullCombo, levelCompletionResults->badCutsCount, levelCompletionResults->missedCount,
//                                                         levelCompletionResults->maxCombo, levelCompletionResults->energy, levelCompletionResults->gameplayModifiers);
//         auto previewBeatmapLevel = reinterpret_cast<IPreviewBeatmapLevel*>(beatmap->get_level());

//         std::string levelHash = GetFormattedHash(previewBeatmapLevel->get_levelID());

//         std::string characteristic = beatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->serializedName;
//         std::string songName = previewBeatmapLevel->get_songName();
//         std::string difficultyName = BeatmapDifficultySerializedMethods::SerializedName(beatmap->get_difficulty());

//         std::string replayFileName = ScoreSaber::Services::FileService::GetReplayFileName(levelHash, difficultyName, characteristic,
//                                                                                           ScoreSaber::Services::PlayerService::playerInfo.localPlayerData.id, songName);
//         Seven(beatmap, levelCompletionResults->modifiedScore, levelCompletionResults->multipliedScore, encryptedPacket, replayFileName);
//     }

//     void Seven(IDifficultyBeatmap* beatmap, int modifiedScore, int multipliedScore, std::string uploadPacket, std::string replayFileName)
//     {
//         HMTask::New_ctor(custom_types::MakeDelegate<System::Action*>((std::function<void()>)[beatmap, modifiedScore, multipliedScore, uploadPacket, replayFileName] {
//             ScoreSaber::UI::Other::ScoreSaberLeaderboardView::SetUploadState(true, false);

//             LeaderboardService::GetLeaderboardData(
//                 beatmap, PlatformLeaderboardsModel::ScoresScope::Global, 1, [=](Data::InternalLeaderboard internalLeaderboard) {
//                     bool ranked = true;
//                     if (internalLeaderboard.leaderboard.has_value())
//                     {
//                         ranked = internalLeaderboard.leaderboard.value().leaderboardInfo.ranked;
//                         if (internalLeaderboard.leaderboard.value().leaderboardInfo.playerScore.has_value())
//                         {
//                             if (modifiedScore < internalLeaderboard.leaderboard.value().leaderboardInfo.playerScore.value().modifiedScore)
//                             {
//                                 ScoreSaber::UI::Other::ScoreSaberLeaderboardView::SetUploadState(false, false, "<color=#fc8181>Didn't beat score, not uploading</color>");
//                                 uploading = false;
//                                 return;
//                             }
//                         }
//                     }
//                     else
//                     {
//                         INFO("Failed to get leaderboards ranked status");
//                     }

//                     bool done = false;
//                     bool failed = false;
//                     int attempts = 0;

//                     auto [beatmapDataBasicInfo, readonlyBeatmapData] = BeatmapUtils::getBeatmapData(beatmap);
//                     int maxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(readonlyBeatmapData);

//                     if(multipliedScore > maxScore) {
//                         ScoreSaber::UI::Other::ScoreSaberLeaderboardView::SetUploadState(false, false, "<color=#fc8181>Failed to upload (score was impossible)</color>");
//                         INFO("Score was better than possible, not uploading!");
//                     }

//                     std::this_thread::sleep_for(std::chrono::milliseconds(1200));
//                     std::string url = BASE_URL + "/api/game/upload";
//                     std::string postData = "data=" + uploadPacket;

//                     while (!done)
//                     {
//                         uploading = true;
//                         INFO("Uploading score...");
//                         auto [responseCode, response] = WebUtils::PostWithReplaySync(url, ReplayService::CurrentSerializedReplay, uploadPacket, 30000);
//                         INFO("Server response:\nHTTP code %ld\nContent: %s", responseCode, response.c_str());
//                         if (responseCode == 200)
//                         {
//                             INFO("Score uploaded successfully");
//                             done = true;
//                         }
//                         else if (responseCode == 403)
//                         {
//                             INFO("Player banned, score didn't upload");
//                             done = true;
//                             failed = true;
//                         }

//                         if (!done)
//                         {
//                             if (attempts < 4)
//                             {
//                                 // Failed but retry
//                                 ERROR("Score failed to upload, retrying");
//                                 attempts++;
//                                 std::this_thread::sleep_for(2000ms);
//                             }
//                             else
//                             {
//                                 done = true;
//                                 failed = true;
//                             }
//                         }

//                     } // We out the loop now

//                     if (done && !failed)
//                     {
//                         // Score uploaded successfully
//                         // Save local replay
//                         INFO("Score uploaded");
//                         SaveReplay(ReplayService::CurrentSerializedReplay, replayFileName);
//                         ScoreSaber::UI::Other::ScoreSaberLeaderboardView::SetUploadState(false, true);
//                     }

//                     if (failed)
//                     {
//                         ERROR("Failed to upload score");
//                         ScoreSaber::UI::Other::ScoreSaberLeaderboardView::SetUploadState(false, false);
//                         // Failed to upload score, tell user
//                     }

//                     uploading = false;
//                 },
//                 false);
//         }), nullptr)->Run();
//     }