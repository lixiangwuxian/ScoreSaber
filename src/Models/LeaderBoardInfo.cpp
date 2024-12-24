#include "shared/Models/LeaderBoardInfo.hpp"

LeaderBoardInfo::LeaderBoardInfo(rapidjson::Value const& document) {
    id = document["id"].IsInt() ? document["id"].GetInt() : 0;
    songHash = document["songHash"].IsString() ? document["songHash"].GetString() : "";
    songName = document["songName"].IsString() ? document["songName"].GetString() : "";
    songSubName = document["songSubName"].IsString() ? document["songSubName"].GetString() : "";
    songAuthorName = document["songAuthorName"].IsString() ? document["songAuthorName"].GetString() : "";
    levelAuthorName = document["levelAuthorName"].IsString() ? document["levelAuthorName"].GetString() : "";
    difficulty = Difficulty(document["difficulty"].GetObject());
    maxScore = document["maxScore"].IsInt() ? document["maxScore"].GetInt() : 0;
    createdDate = document["createdDate"].IsString() ? document["createdDate"].GetString() : "";
    rankedDate = document["rankedDate"].IsString() ? document["rankedDate"].GetString() : "";
    qualifiedDate = document["qualifiedDate"].IsString() ? document["qualifiedDate"].GetString() : "";
    lovedDate = document["lovedDate"].IsString() ? document["lovedDate"].GetString() : "";
    ranked = document["ranked"].IsBool() ? document["ranked"].GetBool() : false;
    qualified = document["qualified"].IsBool() ? document["qualified"].GetBool() : false;
    loved = document["loved"].IsBool() ? document["loved"].GetBool() : false;
    maxPP = document["maxPP"].IsFloat() ? document["maxPP"].GetFloat() : 0;
    stars = document["stars"].IsFloat() ? document["stars"].GetFloat() : 0;
    plays = document["plays"].IsInt() ? document["plays"].GetInt() : 0;
    dailyPlays = document["dailyPlays"].IsInt() ? document["dailyPlays"].GetInt() : 0;
    positiveModifiers = document["positiveModifiers"].IsBool() ? document["positiveModifiers"].GetBool() : false;
    coverImage = document["coverImage"].IsString() ? document["coverImage"].GetString() : "";
}