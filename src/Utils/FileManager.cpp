#include "include/main.hpp"

#include <filesystem>
#include <locale>
#include <fstream>
#include <regex>
#include <codecvt>

#include "include/Utils/FileManager.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

void FileManager::EnsureReplaysFolderExists() {
    string directory = getDataDir(modInfo) + "replays/";
    filesystem::create_directories(directory);
}

void FileManager::WriteReplay(Replay const &replay) {
    ofstream stream = ofstream(ToFilePath(replay), ios::binary);
    replay.Encode(stream);
    stream.flush();
}

std::optional<Metadata> FileManager::ReadInfo(string replayPath) {
    ifstream stream(replayPath, ios::binary);
    return Replay::DecodeInfo(stream);
}

string FileManager::ToFilePath(Replay const &replay) {
    string practice = replay.info.speed > 0.0001 ? "-practice" : "";
    string fail = replay.info.failTime > 0.0001 ? "-fail" : "";
    // TODO: Use fmt
    string filename = replay.info.playerID + std::string(practice + fail) + "-" + replay.info.songName + "-" + replay.info.difficulty + "-" + replay.info.mode + "-" + replay.info.hash + ".bsor";

    string illegalChars = "\\/:?*\"<>()|\r\n";
    for (auto it = filename.begin(); it < filename.end(); ++it){
        bool found = illegalChars.find(*it) != string::npos;
        if(found){
            *it = '_';
        }
    }
    string file = getDataDir(modInfo) + "replays/" + regex_replace(filename, basic_regex("/"), "");;
    // string regexSearch = new string(System::IO::Path::GetInvalidFileNameChars()) + new string(System::IO::Path::GetInvalidPathChars());
    // string reg = "[{" + regexSearch + "}]";
    return file;// regex_replace(filename, basic_regex(reg), "_");
}