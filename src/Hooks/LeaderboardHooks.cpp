#include "Sprites.hpp"
#include "hooks.hpp"

#include "UI/Other/ScoreSaberLeaderboardView.hpp"

// LeaderboardScoreUploader

#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/LeaderboardScoreUploader_ScoreData.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel.hpp"

// StandardLevelScenesTransitionSetupDataSO

#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/LeaderboardTableView_ScoreData.hpp"
#include "GlobalNamespace/LoadingControl.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel_LeaderboardScore.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel_ScoresScope.hpp"
#include "HMUI/SegmentedControl.hpp"
#include "Services/UploadService.hpp"
#include "System/Guid.hpp"
#include "Utils/StringUtils.hpp"

#include "Data/Private/ReplayFile.hpp"
#include "ReplaySystem/Recorders/MainRecorder.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "logging.hpp"

using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace GlobalNamespace;
using namespace ScoreSaber;
using namespace ScoreSaber::UI::Other;
using namespace ScoreSaber::ReplaySystem;
using namespace ScoreSaber::Data::Private;

int _lastScopeIndex = 0;

MAKE_AUTO_HOOK_MATCH(
    PlatformLeaderboardViewController_DidActivate,
    &GlobalNamespace::PlatformLeaderboardViewController::DidActivate, void,
    GlobalNamespace::PlatformLeaderboardViewController* self,
    bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling)
{

    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ResetPage();

    PlatformLeaderboardViewController_DidActivate(self, firstActivation, addedToHeirarchy, screenSystemEnabling);

    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::DidActivate(self, firstActivation, addedToHeirarchy, screenSystemEnabling);

    auto segmentedControl = reinterpret_cast<SegmentedControl*>(self->scopeSegmentedControl);
    segmentedControl->SelectCellWithNumber(_lastScopeIndex);
}

MAKE_AUTO_HOOK_MATCH(PlatformLeaderboardViewController_Refresh,
                     &GlobalNamespace::PlatformLeaderboardViewController::Refresh,
                     void, GlobalNamespace::PlatformLeaderboardViewController* self,
                     bool showLoadingIndicator, bool clear)
{

    // PlatformLeaderboardViewController_Refresh(self, showLoadingIndicator, clear);
    self->hasScoresData = false;
    self->leaderboardTableView->SetScores(System::Collections::Generic::List_1<LeaderboardTableView::ScoreData*>::New_ctor(), -1);
    LoadingControl* loadingControl = self->loadingControl;
    loadingControl->ShowLoading(System::String::_get_Empty());
    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::RefreshLeaderboard(self->difficultyBeatmap, self->leaderboardTableView, self->_get__scoresScope(), loadingControl,
                                                                         StringUtils::Il2cppStrToStr(System::Guid::NewGuid().ToString()));
}

MAKE_AUTO_HOOK_MATCH(PlatformLeaderboardViewController_HandleScopeSegmentedControlDidSelectCell, &GlobalNamespace::PlatformLeaderboardViewController::HandleScopeSegmentedControlDidSelectCell, void,
                     PlatformLeaderboardViewController* self, SegmentedControl* segmentedControl, int cellNumber)
{

    bool filterAroundCountry = false;

    INFO("Scope clicked: %d", cellNumber);

    switch (cellNumber)
    {
        case 0: {
            self->_set__scoresScope(PlatformLeaderboardsModel::ScoresScope::Global);
            break;
        }
        case 1: {
            self->_set__scoresScope(PlatformLeaderboardsModel::ScoresScope::AroundPlayer);
            break;
        }
        case 2: {
            self->_set__scoresScope(PlatformLeaderboardsModel::ScoresScope::Friends);
            break;
        }
        case 3: {
            filterAroundCountry = true;
            break;
        }
    }
    _lastScopeIndex = cellNumber;

    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ChangeScope(filterAroundCountry);
}

MAKE_AUTO_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Finish, &GlobalNamespace::StandardLevelScenesTransitionSetupDataSO::Finish, void,
                     GlobalNamespace::StandardLevelScenesTransitionSetupDataSO* self,
                     GlobalNamespace::LevelCompletionResults* levelCompletionResults)
{
    INFO("StandardLevelScenesTransitionSetupDataSO_Finish hit");
    ScoreSaber::Services::UploadService::Five(self, levelCompletionResults);
    StandardLevelScenesTransitionSetupDataSO_Finish(self, levelCompletionResults);
}

MAKE_AUTO_HOOK_MATCH(PlatformLeaderboardsModel_UploadScore,
                     static_cast<void (GlobalNamespace::PlatformLeaderboardsModel::*)(GlobalNamespace::IDifficultyBeatmap*, int, int, bool, int, int, int, int, float, GlobalNamespace::GameplayModifiers*)>(&GlobalNamespace::PlatformLeaderboardsModel::UploadScore),
                     void, GlobalNamespace::PlatformLeaderboardsModel* self, GlobalNamespace::IDifficultyBeatmap* beatmap, int rawScore,
                     int modifiedScore, bool fullCombo, int goodCutsCount, int badCutsCount, int missedCount, int maxCombo,
                     float energy, GlobalNamespace::GameplayModifiers* gameplayModifiers)
{
    // ScoreSaber::Services::UploadService::PrepareAndUploadScore(beatmap, rawScore, modifiedScore, fullCombo, goodCutsCount, badCutsCount, missedCount, maxCombo, energy, gameplayModifiers);
}