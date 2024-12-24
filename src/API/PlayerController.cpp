#include "include/API/PlayerController.hpp"
#include "include/Utils/WebUtils.hpp"
#include "include/Utils/ModConfig.hpp"
#include "include/Utils/StringUtils.hpp"
#include "include/main.hpp"
#include "include/UI/LeaderboardUI.hpp"

#include "GlobalNamespace/IPlatformUserModel.hpp"
#include "GlobalNamespace/UserInfo.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel.hpp"

#include "System/Action_1.hpp"
#include "System/Threading/Tasks/Task.hpp"
#include "System/Threading/Tasks/Task_1.hpp"

#include "UnityEngine/Resources.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

using UnityEngine::Resources;
using namespace GlobalNamespace;
using namespace rapidjson;

optional<Player> PlayerController::currentPlayer = nullopt;
string PlayerController::lastErrorDescription = "";
vector<function<void(optional<Player> const&)>> PlayerController::playerChanged;

void callbackWrapper(optional<Player> const& player) {
    for (auto && fn : PlayerController::playerChanged)
        fn(player);
}

void PlayerController::Refresh(int retry, const function<void(optional<Player> const&, string)>& finished) {
    //getSecretFile
    string secretFile = WebUtils::getSecretFile();
    if (secretFile.empty()) {
        finished(nullopt, "Secret file not found");
        return;
    }
    __sFILE * file = fopen(secretFile.c_str(), "r");
    if (file == nullptr) {
        finished(nullopt, "Secret file not found");
        return;
    }
    char buffer[50];
    string secretKey = fgets(buffer, sizeof(buffer), file);
    fclose(file);
    if (secretKey.empty()) {
        finished(nullopt, "Secret file is empty");
        return;
    }
    
    size_t delimiterPos = secretKey.find(':');
    if (delimiterPos == string::npos) {
        lastErrorDescription = "Invalid format, expected 'steamKey:playerId'";
        ScoreSaberLogger.error("ScoreSaber {}", lastErrorDescription.c_str());
        finished(nullopt, lastErrorDescription);
        return;
    }
    
    string steamKey = secretKey.substr(0, delimiterPos);
    string playerId = secretKey.substr(delimiterPos + 1);
    auto handleError = [retry, finished,steamKey,playerId](){
        if (retry < 3) {
            LogIn(steamKey+":"+playerId, finished);
        } else {
            currentPlayer = nullopt;
            if (finished) finished(nullopt, "Failed to retrieve player");
        }
    };
    auto allFinished = [finished,handleError](auto player, auto str){
        if (player) {
            finished(player, "");
        } else {
            handleError();
        }
    };
    GetPlayerInfo(playerId, true, allFinished);
}

//implement by gpt-4o
void PlayerController::LogIn(string steamKeyAndUserId, const function<void(optional<Player> const&, string)>& finished) {
    lastErrorDescription = "";
    // 分割登录数据
    size_t delimiterPos = steamKeyAndUserId.find(':');
    if (delimiterPos == string::npos) {
        lastErrorDescription = "Invalid login format, expected 'steamKey:playerId'";
        ScoreSaberLogger.error("BeatLeader {}", lastErrorDescription.c_str());
        finished(nullopt, lastErrorDescription);
        return;
    }
    
    string steamKey = steamKeyAndUserId.substr(0, delimiterPos);
    string playerId = steamKeyAndUserId.substr(delimiterPos + 1);

    // 校验登录数据是否有效
    if (steamKey.empty() || playerId.empty()) {
        lastErrorDescription = "Missing steamKey or playerId";
        ScoreSaberLogger.error("BeatLeader {}", lastErrorDescription.c_str());
        finished(nullopt, lastErrorDescription);
        return;
    }
    
    string url = WebUtils::API_URL + "/api/game/auth";

    // 异步POST请求
    WebUtils::PostFormAsync(url, steamKey,playerId,"login", [finished,url,playerId,steamKeyAndUserId](long statusCode, string response) {
        if (statusCode == 200) {
            try {
                rapidjson::Document jsonDocument;
                jsonDocument.Parse(response.c_str());
                if (jsonDocument.HasParseError() || !jsonDocument.IsObject()) {
                    string error = "Failed to parse login response";
                    ScoreSaberLogger.error("BeatLeader {}", error.c_str());
                    finished(nullopt, error);
                    return;
                }

                Session session(jsonDocument.GetObject());

                GetPlayerInfo(playerId, true, finished);
                string secretFile = WebUtils::getSecretFile();
                if (secretFile.empty()) {
                    finished(nullopt, "Secret file not found");
                    return;
                }
                __sFILE * file = fopen(secretFile.c_str(), "w");
                if (file == nullptr) {
                    finished(nullopt, "Secret file not found");
                    return;
                }
                fputs(steamKeyAndUserId.c_str(), file);
                fclose(file);

            } catch (const std::exception& e) {
                string error = "Exception during login: " + string(e.what());
                ScoreSaberLogger.error("BeatLeader {}", error.c_str());
                finished(nullopt, error);
            }
        } else {
            string error = "Login failed "+playerId+" "+ to_string(statusCode)+" "+response;
            lastErrorDescription = error;
            ScoreSaberLogger.error("BeatLeader {}", error.c_str());
            finished(nullopt, error);
        }
    });
}


void PlayerController::GetPlayerInfo(string playerId, bool full,const function<void(optional<Player> const&, string)>& finished) {
    string url = WebUtils::API_URL + "/api/player/" + playerId;
    if (full) {
        url = url + "/full";
    } else {
        url = url + "/basic";
    }
    WebUtils::GetJSONAsync(url, [finished](long status, bool error, rapidjson::Document const& result){
        if (status == 200 && !error) {
            // Player player(result.GetObject());
            currentPlayer = Player(result.GetObject());
            finished(optional<Player>(currentPlayer), "");
        }
        else {
            finished(nullopt, "Failed to retrieve player");
        }
    });
}


// void PlayerController::LogOut() {
//     WebUtils::GetAsync(WebUtils::API_URL + "signout", [](long statusCode, string error) {});
//     remove(WebUtils::getCookieFile().data());

//     currentPlayer = nullopt;
//     callbackWrapper(currentPlayer);
// }

// bool PlayerController::IsFriend(Player anotherPlayer) {
//     if (currentPlayer == nullopt) return false;

//     return std::find(currentPlayer->friends.begin(), currentPlayer->friends.end(), anotherPlayer.id) != currentPlayer->friends.end();
// }

// bool PlayerController::InClan(string tag) {
//     if (currentPlayer == nullopt) return false;

//     auto it = std::find_if(currentPlayer->clans.begin(), currentPlayer->clans.end(), 
//                            [&tag](const auto& clan) { return toLower(clan.tag) == toLower(tag); });

//     return it != currentPlayer->clans.end();
// }

// bool PlayerController::IsMainClan(string tag) {
//     if (currentPlayer == nullopt || currentPlayer->clans.size() == 0) return false;

//     return toLower(currentPlayer->clans[0].tag) == toLower(tag);
// }

bool PlayerController::IsIncognito(Player anotherPlayer) {
    Document incognitoList;
    incognitoList.Parse(getModConfig().IncognitoList.GetValue().c_str());

    if (incognitoList.HasParseError() || !incognitoList.IsObject()) {
        getModConfig().IncognitoList.SetValue("{}");
        return false;
    }

    auto incognitoArray = incognitoList.GetArray();

    for (int index = 0; index < (int)incognitoArray.Size(); ++index) {
        auto const& id = incognitoArray[index].GetString();
        if (strcmp(id, anotherPlayer.id.c_str()) == 0) {
            return true;
        }
    }
    
    return false;
}

void PlayerController::SetIsIncognito(Player anotherPlayer, bool value) {
    Document incognitoList;
    incognitoList.Parse(getModConfig().IncognitoList.GetValue().c_str());

    if (incognitoList.HasParseError() || !incognitoList.IsObject()) {
        getModConfig().IncognitoList.SetValue("{}");
        return;
    }

    auto incognitoArray = incognitoList.GetArray();
    
    rapidjson::Document::AllocatorType& allocator = incognitoList.GetAllocator();
    if (value) {
        Value rj_key;
        rj_key.SetString(anotherPlayer.id.c_str(), anotherPlayer.id.length(), allocator);
        incognitoList.PushBack(rj_key, allocator);
    } else {
        for (int idx = 0 ; idx < (int) incognitoList.Size() ; idx++) {
            if (strcmp(incognitoList[idx].GetString(), anotherPlayer.id.c_str()) == 0) {
                incognitoList.Erase(incognitoList.Begin() + idx--);
                break;
            }
        }
    }

    StringBuffer buffer;
    buffer.Clear();
    Writer<StringBuffer> writer(buffer);
    if (incognitoList.Accept(writer)) {
        string incognitoListString = string(buffer.GetString());
        getModConfig().IncognitoList.SetValue(incognitoListString);
    }
}

bool PlayerController::IsPatron(Player anotherPlayer) {
    return 
        anotherPlayer.role.find("tipper") != string::npos ||
        anotherPlayer.role.find("supporter") != string::npos ||
        anotherPlayer.role.find("sponsor") != string::npos;
}
