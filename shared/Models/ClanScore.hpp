#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "shared/Models/Clan.hpp"

#include <string>
using namespace std;

struct ClanScore
{
    int id;
    int modifiedScore;
    float accuracy;
    int clanId;
    float pp;
    int rank;
    string timeset;

    Clan clan;
    ClanScore();
    ClanScore(rapidjson::Value const& document);
};