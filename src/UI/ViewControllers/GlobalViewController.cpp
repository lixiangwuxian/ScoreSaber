#include "UI/ViewControllers/GlobalViewController.hpp"

#include "CustomTypes/Components/GlobalLeaderboardTableCell.hpp"
#include "CustomTypes/Components/GlobalLeaderboardTableData.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "HMUI/TableView_ScrollPositionType.hpp"

#include "Sprites.hpp"
#include "UnityEngine/Application.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "Utils/UIUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/QuestUI.hpp"

DEFINE_TYPE(ScoreSaber::UI::ViewControllers, GlobalViewController);

using namespace ScoreSaber::CustomTypes::Components;
using namespace TMPro;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;

custom_types::Helpers::Coroutine WaitForInit(
    SafePtrUnity<ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData>
        leaderboard,
    std::function<void()> callback)
{
    while (!leaderboard->initialized)
    {
        co_yield nullptr;
    }
    callback();
    co_return;
}

#define SetPreferredSize(identifier, width, height)                                           \
    auto layout##identifier = identifier->get_gameObject() -> GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                  \
        layout##identifier = identifier->get_gameObject()->AddComponent<LayoutElement*>();    \
    layout##identifier->set_preferredWidth(width);                                            \
    layout##identifier->set_preferredHeight(height)

namespace ScoreSaber::UI::ViewControllers
{
    void GlobalViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            VerticalLayoutGroup* vertical = BeatSaberUI::CreateVerticalLayoutGroup(get_transform());
            vertical->set_spacing(2.0f);
            vertical->set_childControlHeight(false);
            vertical->set_childAlignment(TextAnchor::MiddleCenter);

            ContentSizeFitter* verticalFitter = vertical->GetComponent<ContentSizeFitter*>();
            verticalFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
            verticalFitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);

            LayoutElement* verticalElement = vertical->GetComponent<LayoutElement*>();
            verticalElement->set_preferredWidth(120.0f);

            auto headerHorizontal = CreateHorizontalLayoutGroup(vertical->get_transform());
            SetPreferredSize(headerHorizontal, 120.0f, -1);
            headerHorizontal->set_childControlHeight(true);
            auto textHorizontal = CreateHorizontalLayoutGroup(headerHorizontal->get_transform());
            textHorizontal->set_childForceExpandWidth(false);
            textHorizontal->set_childAlignment(TextAnchor::MiddleCenter);
            SetPreferredSize(textHorizontal, 80.0f, -1);
            textHorizontal->get_rectTransform()->set_sizeDelta(Vector2(-40.0f, 0.0f));
            auto headerText = UIUtils::CreateClickableText(textHorizontal->get_transform(), "全球排行榜", Vector2(0.0f, 0.0f), Vector2(0.0f, 0.0f), []() { Application::OpenURL("https://scoresaber.com/rankings"); });

            headerText->set_alignment(TMPro::TextAlignmentOptions::Center);
            headerText->set_fontSize(7.0f);

            auto infoButton = CreateUIButton(headerHorizontal->get_transform(), " ?", Vector2(54.0f, 0.0f), Vector2(10.0f, 8.5f), std::bind(&GlobalViewController::OpenMoreInfoModal, this));

            auto layoutinfoButton = infoButton->GetComponent<LayoutElement*>();
            layoutinfoButton->set_ignoreLayout(true);
            auto textObject = infoButton->GetComponentInChildren<TMPro::TextMeshProUGUI*>();
            textObject->set_alignment(TMPro::TextAlignmentOptions::Left);

            auto headerBG = headerHorizontal->get_gameObject()->AddComponent<Backgroundable*>();
            headerBG->ApplyBackgroundWithAlpha("round-rect-panel", 0.5f);
            auto headerImageView = headerBG->get_gameObject()->GetComponentInChildren<HMUI::ImageView*>()->skew = 0.18f;

            HorizontalLayoutGroup* globalHost = BeatSaberUI::CreateHorizontalLayoutGroup(vertical->get_transform());
            globalHost->set_spacing(1.0f);

            ContentSizeFitter* globalHostFitter = globalHost->GetComponent<ContentSizeFitter*>();
            globalHostFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
            globalHostFitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);

            LayoutElement* globalHostElement = globalHost->GetComponent<LayoutElement*>();
            globalHostElement->set_preferredWidth(120.0f);

            VerticalLayoutGroup* scoreScopesHost = BeatSaberUI::CreateVerticalLayoutGroup(globalHost->get_transform());

            LayoutElement* scoreScopesHostElement = scoreScopesHost->GetComponent<LayoutElement*>();
            scoreScopesHostElement->set_preferredWidth(9.0f);

            auto arrow = UIUtils::CreateClickableImage(scoreScopesHost->get_transform(), Base64ToSprite(carat_up_base64), {0.0f, 25.0f}, {9.0f, 9.0f},
                                                       std::bind(&GlobalViewController::UpButtonWasPressed, this));

            VerticalLayoutGroup* scoreScopes = BeatSaberUI::CreateVerticalLayoutGroup(scoreScopesHost->get_transform());

            RectOffset* scoreScopesPad = scoreScopes->get_padding();
            scoreScopesPad->set_bottom(1);
            scoreScopesPad->set_left(1);
            scoreScopesPad->set_right(1);
            scoreScopesPad->set_top(1);

            LayoutElement* scoreScopesElement = scoreScopes->GetComponent<LayoutElement*>();
            scoreScopesElement->set_preferredWidth(9.0f);
            scoreScopesElement->set_preferredHeight(40.0f);

            Backgroundable* background = scoreScopes->get_gameObject()->AddComponent<Backgroundable*>();
            background->ApplyBackgroundWithAlpha("round-rect-panel", 1.0f);

            VerticalLayoutGroup* imagesGroup = BeatSaberUI::CreateVerticalLayoutGroup(scoreScopes->get_transform());

            LayoutElement* imagesGroupElement = imagesGroup->GetComponent<LayoutElement*>();
            imagesGroupElement->set_preferredWidth(4.0f);
            imagesGroupElement->set_preferredHeight(20.0f);
            imagesGroup->set_spacing(2);
            auto countrySprite = Base64ToSprite(country_base64);
            auto globalSprite = Base64ToSprite(global_base64);
            auto playerSprite = Base64ToSprite(player_base64);
            auto friendsSprite = Base64ToSprite(friends_base64);

            auto globalButton = UIUtils::CreateClickableImage(imagesGroup->get_transform(),
                                                              globalSprite, {0.0f, 0.0f},
                                                              {4.0f, 4.0f}, std::bind(&GlobalViewController::FilterWasClicked, this, ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData::LeaderboardType::Global));
            globalButton->set_preserveAspect(true);
            auto aroundYouButton = UIUtils::CreateClickableImage(imagesGroup->get_transform(),
                                                                 playerSprite, {0.0f, 0.0f},
                                                                 {4.0f, 4.0f}, std::bind(&GlobalViewController::FilterWasClicked, this, ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData::LeaderboardType::AroundYou));
            aroundYouButton->set_preserveAspect(true);
            auto friendsButton = UIUtils::CreateClickableImage(imagesGroup->get_transform(),
                                                               friendsSprite, {0.0f, 0.0f},
                                                               {4.0f, 4.0f}, std::bind(&GlobalViewController::FilterWasClicked, this, ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData::LeaderboardType::Friends));
            friendsButton->set_preserveAspect(true);
            auto countryButton = UIUtils::CreateClickableImage(imagesGroup->get_transform(),
                                                               countrySprite, {0.0f, 0.0f},
                                                               {4.0f, 4.0f}, std::bind(&GlobalViewController::FilterWasClicked, this, ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData::LeaderboardType::Country));
            countryButton->set_preserveAspect(true);

            auto downrrow = UIUtils::CreateClickableImage(scoreScopesHost->get_transform(), Base64ToSprite(carat_down_base64),
                                                          {0.0f, 25.0f}, {9.0f, 9.0f}, std::bind(&GlobalViewController::DownButtonWasPressed, this));

            VerticalLayoutGroup* globalVerticalHost = BeatSaberUI::CreateVerticalLayoutGroup(globalHost->get_transform());

            VerticalLayoutGroup* playersHost = BeatSaberUI::CreateVerticalLayoutGroup(globalVerticalHost->get_transform());

            auto loadingVertical = CreateVerticalLayoutGroup(get_transform());
            SetPreferredSize(loadingVertical, 10, 10);
            layoutloadingVertical->set_ignoreLayout(true);
            loadingVertical->get_rectTransform()->set_anchoredPosition(playersHost->get_rectTransform()->get_anchoredPosition());
            auto loadingHorizontal = CreateHorizontalLayoutGroup(loadingVertical->get_transform());
            SetPreferredSize(loadingHorizontal, 10, 10);

            loadingIndicator = UIUtils::CreateLoadingIndicator(loadingHorizontal->get_transform());
            auto loadingIndicatorLayout = loadingIndicator->GetComponent<LayoutElement*>();
            auto loadingIndicatorRectTransform = reinterpret_cast<RectTransform*>(loadingIndicator->get_transform());

            playersHost->set_padding(RectOffset::New_ctor(2, 2, 2, 2));

            Backgroundable* playersHostBg = playersHost->get_gameObject()->AddComponent<Backgroundable*>();
            playersHostBg->ApplyBackgroundWithAlpha("round-rect-panel", 1.0f);

            LayoutElement* playersHostElement = playersHost->GetComponent<LayoutElement*>();
            playersHostElement->set_preferredWidth(105.0f);
            playersHostElement->set_preferredHeight(60.0f);

            auto sizeDelta = Vector2(105.0f, 60.0f);
            auto anchoredPosition = Vector2(0.0f, 0.0f);
            playersHost->get_rectTransform()->set_sizeDelta(sizeDelta);
            playersHost->get_rectTransform()->set_anchoredPosition(anchoredPosition);
            playersHost->set_childForceExpandHeight(false);
            playersHost->set_childControlHeight(false);
            playersHost->set_childScaleHeight(false);

            leaderboardList = CreateCustomSourceList<ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData*>(playersHost->get_transform(), Vector2(sizeDelta.x, sizeDelta.y), nullptr);
            leaderboardList->globalViewController = this;
            leaderboardList->StartRefresh();

            playerProfileModal = ::ScoreSaber::UI::Other::PlayerProfileModal::Create(get_transform());
            leaderboardList->playerProfileModal = playerProfileModal;
        }
    }

    void GlobalViewController::set_loading(bool value)
    {
        loadingIndicator->SetActive(value);
    }

    void GlobalViewController::UpButtonWasPressed()
    {
        leaderboardList->UpButtonWasPressed();
    }

    void GlobalViewController::DownButtonWasPressed()
    {
        leaderboardList->DownButtonWasPressed();
    }

    void GlobalViewController::OpenMoreInfoModal()
    {
        if (!moreInfoModal)
        {
            // make it
            moreInfoModal = CreateModal(get_transform(), Vector2(84.0f, 64.0f), nullptr, true);
            auto textVertical = CreateVerticalLayoutGroup(moreInfoModal->get_transform());
            textVertical->set_spacing(1.0f);
            textVertical->set_padding(RectOffset::New_ctor(2, 2, 2, 2));
            auto h1 = CreateText(textVertical->get_transform(), "<color=#6772E5>PP</color>是什么?", false);
            h1->set_alignment(TMPro::TextAlignmentOptions::TopLeft);
            h1->set_fontSize(4.0f);
            auto t1 = CreateText(textVertical->get_transform(), "表现分(<color=#6772E5>PP</color>)可通过游玩ranked类型的谱面获取。<color=#6772E5>PP</color>高低与谱面难度\n"
                                                                "及您在这个谱面取得的分数正相关。您在全球排行榜上的位置取决于您的PP，\n"
                                                                "您的<color=#6772E5>PP</color>越高，您的排名也就越高\n",
                                 false);
            t1->set_alignment(TMPro::TextAlignmentOptions::TopLeft);
            t1->set_fontSize(2.5f);
            t1->set_enableWordWrapping(true);
            t1->get_rectTransform()->set_sizeDelta({0, 0});
            Object::Destroy(t1->GetComponent<LayoutElement*>());
            auto h2 = CreateText(textVertical->get_transform(), "Ranking系统的原理是?", false);
            h2->set_alignment(TMPro::TextAlignmentOptions::TopLeft);
            h2->set_fontSize(4.0f);
            auto t2 = CreateText(textVertical->get_transform(), "Ranked谱面是通过了Ranking系统的谱面，该系统会对谱面进行判断以确保它们相对合理。\n"
                                                                "Ranking团队会根据Ranking标准对谱面进行投票。Ranking标准是一个概述了谱面需要成为\n"
                                                                "Ranked状态所必须遵循的规则的文档。质量保证团队会根据谱面是否符合他们的质量标准\n"
                                                                "来投票提前或推迟位于等待Ranking队列中的谱面成为Ranked谱面，但是他们不能彻底阻止\n"
                                                                "符合标准的谱面成为Ranked谱面。",
                                 false);
            t2->set_alignment(TMPro::TextAlignmentOptions::TopLeft);
            t2->set_fontSize(2.5f);
            t2->set_enableWordWrapping(true);
            t2->get_rectTransform()->set_sizeDelta({0, 0});
            Object::Destroy(t2->GetComponent<LayoutElement*>());

            auto buttonHorizontal = CreateHorizontalLayoutGroup(textVertical->get_transform());
            auto dismiss = CreateUIButton(buttonHorizontal->get_transform(), "取消", [&]() { moreInfoModal->Hide(true, nullptr); });

            auto moreInfo = CreateUIButton(buttonHorizontal->get_transform(), "更多信息", []() { Application::OpenURL("https://wiki.scoresaber.com/ranking-system.html"); });
        }
        moreInfoModal->Show(true, true, nullptr);
    }

    void GlobalViewController::FilterWasClicked(ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData::LeaderboardType type)
    {
        leaderboardList->set_leaderboardType(type);
    }
} // namespace ScoreSaber::UI::ViewControllers
