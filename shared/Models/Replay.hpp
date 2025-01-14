#pragma once

#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

#include "sombrero/shared/FastVector3.hpp"
#include "sombrero/shared/FastQuaternion.hpp"
#include "sombrero/shared/Vector3Utils.hpp"


using namespace std;

struct Metadata {
    string version;
    string gameVersion;
    string timestamp;


    string playerID;
    string playerName;
    string platform;

    string trackingSystem;
    string hmd;
    string controller;

    string hash;
    string songName;
    string mapper;
    string difficulty;

    int score;
    string mode;
    string environment;
    string modifiers;
    float jumpDistance = 0;
    bool leftHanded = false;
    float height = 0;

    float startTime = 0;
    float failTime = 0;
    float speed = 0;

    
    
    Metadata(string version, string gameVersion, string timestamp) : Version(version) {
        Version = version;
    }
    
    std::string Version;
    std::string LevelID;
    int Difficulty;
    std::string Characteristic;
    std::string Environment;
    std::vector<std::string> Modifiers;
    float NoteSpawnOffset;
    bool LeftHanded;
    float InitialHeight;
    float RoomRotation;
    // VRPosition RoomCenter;
    float FailTime;
};

struct ReplayTransform {
    Sombrero::FastVector3 position;
    Sombrero::FastQuaternion rotation;

    constexpr ReplayTransform(Sombrero::FastVector3 const &position, Sombrero::FastQuaternion const &rotation) : position(position),
                                                                                                 rotation(rotation) {}
};

struct Frame {
    float time;
    int fps;
    ReplayTransform head;
    ReplayTransform leftHand;
    ReplayTransform rightHand;

    constexpr Frame(float time, int fps, ReplayTransform const &head, ReplayTransform const &leftHand, ReplayTransform const &rightHand) : time(
            time), fps(fps), head(head), leftHand(leftHand), rightHand(rightHand) {}
};

struct ReplayNoteCutInfo {
    bool speedOK;
    bool directionOK;
    bool saberTypeOK;
    bool wasCutTooSoon;
    float saberSpeed;
    bool cutDistanceToCenterPositive;
    Sombrero::FastVector3 saberDir;
    int saberType;
    float timeDeviation;
    float cutDirDeviation;
    Sombrero::FastVector3 cutPoint;
    Sombrero::FastVector3 cutNormal;
    float cutDistanceToCenter;
    float cutAngle;
    float beforeCutRating;
    float afterCutRating;
};

enum struct NoteEventType {
    NONE = 0,
    GOOD = 1,
    BAD = 2,
    MISS = 3,
    BOMB = 4
};

struct NoteID
{
    NoteID()=default;
    NoteID(float Time, int LineLayer, int LineIndex, int ColorType, int CutDirection):Time(Time), LineLayer(LineLayer), LineIndex(LineIndex), ColorType(ColorType), CutDirection(CutDirection) {}
    NoteID(float Time, int LineLayer, int LineIndex, int ColorType, int CutDirection, int GameplayType, int ScoringType, int CutDirectionAngleOffset):Time(Time), LineLayer(LineLayer), LineIndex(LineIndex), ColorType(ColorType), CutDirection(CutDirection), GameplayType(GameplayType), ScoringType(ScoringType), CutDirectionAngleOffset(CutDirectionAngleOffset) {}
    float Time;
    int LineLayer;
    int LineIndex;
    int ColorType;
    int CutDirection;
    std::optional<int> GameplayType;
    std::optional<int> ScoringType;
    std::optional<float> CutDirectionAngleOffset;
};


struct VRPosition
{
    VRPosition()=default;
    VRPosition(float X, float Y, float Z):X(X), Y(Y), Z(Z) {}
    VRPosition(UnityEngine::Vector3 vector):X(vector.x), Y(vector.y), Z(vector.z) {}
    float X;
    float Y;
    float Z;
};


struct NoteEvent {
    NoteID TheNoteID;
    NoteEventType EventType; // GOOD, BAD, MISS
    VRPosition CutPoint;
    VRPosition CutNormal;
    VRPosition SaberDirection;
    int SaberType;
    bool DirectionOK;
    float SaberSpeed;
    float CutAngle;
    float CutDistanceToCenter;
    float CutDirectionDeviation;
    float BeforeCutRating;
    float AfterCutRating;
    float Time;
    float UnityTimescale;
    float TimeSyncTimescale;
};

struct EnergyEvent {
    float Energy;
    float Time;

    constexpr EnergyEvent(float Energy, float Time) : Energy(Energy), Time(Time) {}
};

struct HeightEvent {
    constexpr HeightEvent(float height, float time) : height(height), time(time) {}

    float height;
    float time;
};

class Replay
{
public:
    void Encode(ofstream& stream) const;
    static std::optional<Metadata> DecodeInfo(ifstream& stream);

    explicit Replay(Metadata info) : info(std::move(info)) {}
    Replay(Replay&&) = default; // make move default, avoid copying


    Metadata info;
    vector<Frame> frames;
    vector<NoteEvent> notes;
    vector<HeightEvent> heights;
private:
    static void Encode(char value, ofstream& stream);
    static void Encode(int value, ofstream& stream);
    static void Encode(long value, ofstream& stream);
    static void Encode(bool value, ofstream& stream);
    static void Encode(float value, ofstream& stream);
    static void Encode(string value, ofstream& stream);

    // template methods MUST be header only
    template<class T>
    static void Encode(vector<T> const& list, ofstream& stream) {
        size_t count = list.size();
        Encode((int)count, stream);
        for (size_t i = 0; i < count; i++)
        {
            Encode(list[i], stream);
        }
    }

    static void Encode(Metadata const &info, ofstream& stream);
    static void Encode(Sombrero::FastVector3 const &vector, ofstream& stream);
    static void Encode(Sombrero::FastQuaternion const &quaternion, ofstream& stream);
    static void Encode(Frame const &frame, ofstream& stream);
    static void Encode(NoteEvent const &note, ofstream& stream);
    static void Encode(HeightEvent const &height, ofstream& stream);

    static char DecodeChar(ifstream& stream);
    static int DecodeInt(ifstream& stream);
    static long DecodeLong(ifstream& stream);
    static bool DecodeBool(ifstream& stream);
    static float DecodeFloat(ifstream& stream);
    static string DecodeString(ifstream& stream);
};