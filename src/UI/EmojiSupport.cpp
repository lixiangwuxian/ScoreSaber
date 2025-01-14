#include "include/UI/EmojiSupport.hpp"
#include "include/Assets/Sprites.hpp"
#include "include/Assets/BundleLoader.hpp"
#include "include/Utils/WebUtils.hpp"

#include "UnityEngine/Texture.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "UnityEngine/ImageConversion.hpp"
#include "UnityEngine/Graphics.hpp"
#include "UnityEngine/TextCore/GlyphMetrics.hpp"
#include "UnityEngine/TextCore/GlyphRect.hpp"
#include "UnityEngine/TextCore/Text/FontAssetUtilities.hpp"
#include "UnityEngine/TextureFormat.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"

#include "TMPro/TMP_SpriteAsset.hpp"
#include "TMPro/TMP_Sprite.hpp"
#include "TMPro/TMP_TextUtilities.hpp"
#include "TMPro/TMP_SpriteGlyph.hpp"
#include "TMPro/TMP_SpriteCharacter.hpp"
#include "TMPro/TMP_FontAssetUtilities.hpp"

#include "main.hpp"

#include <sstream>

const int EMOJI_SIZE = 72;
const int SHEET_TILES = 10;
const int SHEET_SIZE = SHEET_TILES * EMOJI_SIZE;

static bool hooksInstalled = false;

static int currentEmojiIndex;
static bool textureNeedsApply;
static ArrayW<UnityEngine::Color> clearPixels;
static TMPro::TMP_SpriteAsset* rootEmojiAsset;
static TMPro::TMP_SpriteAsset* currentEmojiAsset;

TMPro::TextMeshProUGUI* lastText;
static vector<TMPro::TextMeshProUGUI*> textToUpdate;
static int loadingCount = 0;
  
std::string utf8ToInt(int unicode) {
    std::stringstream strm;
    strm << unicode;
    return strm.str(); 
} 

std::string utf8ToHex(int unicode) { 
    std::stringstream stream;
    stream << std::hex << unicode;
    return stream.str();
}

TMPro::TMP_SpriteAsset* CreateTMP_SpriteAsset() {
    auto texture = UnityEngine::Texture2D::New_ctor(SHEET_SIZE, SHEET_SIZE, UnityEngine::TextureFormat::RGBA32, false);
    texture->SetPixels(clearPixels);
    texture->Apply(false, true);

    TMPro::TMP_SpriteAsset* spriteAsset = ScriptableObject::CreateInstance<TMPro::TMP_SpriteAsset*>();
    spriteAsset->fallbackSpriteAssets = System::Collections::Generic::List_1<UnityW<TMPro::TMP_SpriteAsset>>::New_ctor();
    spriteAsset->spriteInfoList = System::Collections::Generic::List_1<TMPro::TMP_Sprite*>::New_ctor();
    spriteAsset->spriteSheet = texture;
    spriteAsset->material =  UnityEngine::Material::New_ctor(BundleLoader::bundle->TMP_SpriteCurved);
    spriteAsset->material->set_mainTexture(spriteAsset->spriteSheet);

    return spriteAsset;
}

void DrawSprite(
    UnityEngine::Texture* emojiTex,
    int emojiIndex, 
    TMPro::TMP_SpriteGlyph* glyph,
    TMPro::TMP_SpriteAsset* emojiAsset) {
    int row = emojiIndex % SHEET_TILES;
    int column = emojiIndex / SHEET_TILES;
    int width = emojiTex->get_width();
    static auto CopyTexture_Region = reinterpret_cast<function_ptr_t<void, UnityEngine::Texture*, int, int, int, int, int, int, UnityEngine::Texture*, int, int, int, int>>(il2cpp_functions::resolve_icall("UnityEngine.Graphics::CopyTexture_Region"));
    CopyTexture_Region(emojiTex, 0, 0, 0, 0, width, emojiTex->get_height(),
                                currentEmojiAsset->spriteSheet, 0, 0, row * EMOJI_SIZE,
                                (SHEET_SIZE) - ((column + 1) * EMOJI_SIZE));
    glyph->set_metrics(UnityEngine::TextCore::GlyphMetrics(width, EMOJI_SIZE, 0.25f * width, EMOJI_SIZE * 0.75f, width));
    glyph->set_glyphRect(UnityEngine::TextCore::GlyphRect(row * EMOJI_SIZE, (SHEET_SIZE) - ((column + 1) * EMOJI_SIZE), width, EMOJI_SIZE));

    emojiAsset->SortGlyphTable();
    emojiAsset->UpdateLookupTables();
}

TMPro::TMP_SpriteGlyph* PushSprite(int unicode) {
    if (currentEmojiIndex >= SHEET_TILES * SHEET_TILES) {
        auto newSheet = CreateTMP_SpriteAsset();
        rootEmojiAsset->fallbackSpriteAssets->Add(newSheet);
        currentEmojiAsset = newSheet;
        currentEmojiIndex = 0;
    }

    TMPro::TMP_Sprite* tmp_Sprite = TMPro::TMP_Sprite::New_ctor();
    string text = utf8ToInt(unicode);

    int row = currentEmojiIndex % SHEET_TILES;
    int column = currentEmojiIndex / SHEET_TILES;

    TMPro::TMP_SpriteGlyph* sprite = TMPro::TMP_SpriteGlyph::New_ctor();
    sprite->set_index(currentEmojiIndex);
    sprite->set_metrics(UnityEngine::TextCore::GlyphMetrics(EMOJI_SIZE, EMOJI_SIZE, 0.25f * EMOJI_SIZE, EMOJI_SIZE * 0.75f, EMOJI_SIZE));
    sprite->set_glyphRect(UnityEngine::TextCore::GlyphRect(row * EMOJI_SIZE, (SHEET_SIZE) - ((column + 1) * EMOJI_SIZE), EMOJI_SIZE, EMOJI_SIZE));

    if (currentEmojiAsset->m_SpriteGlyphTable == NULL) {
        currentEmojiAsset->m_SpriteGlyphTable = System::Collections::Generic::List_1<::TMPro::TMP_SpriteGlyph*>::New_ctor();
    }
    currentEmojiAsset->m_SpriteGlyphTable->Add(sprite);
    
    TMPro::TMP_SpriteCharacter* character = TMPro::TMP_SpriteCharacter::New_ctor(unicode, sprite);
    character->set_name(text);
    character->set_scale(1);

    if (currentEmojiAsset->m_SpriteCharacterTable == NULL) {
        currentEmojiAsset->m_SpriteCharacterTable = System::Collections::Generic::List_1<::TMPro::TMP_SpriteCharacter*>::New_ctor();
    }
    currentEmojiAsset->m_SpriteCharacterTable->Add(character);

    currentEmojiAsset->SortGlyphTable();
    currentEmojiAsset->UpdateLookupTables();

    return sprite;
}


MAKE_HOOK_MATCH(GetSpriteCharacterFromSpriteAsset, &TMPro::TMP_FontAssetUtilities::GetSpriteCharacterFromSpriteAsset, TMPro::TMP_SpriteCharacter*, uint32_t unicode, TMPro::TMP_SpriteAsset* spriteAsset, bool includeFallbacks) {
    auto result = GetSpriteCharacterFromSpriteAsset(unicode, spriteAsset, includeFallbacks);
    if (result == NULL) {
        auto glyph = PushSprite(unicode);

        int indexToUse = currentEmojiIndex;
        auto assetToUse = currentEmojiAsset;
        loadingCount++;

        Sprites::get_Icon(WebUtils::API_URL + "unicode/" + utf8ToHex(unicode) + ".png", [indexToUse, assetToUse, glyph, unicode](UnityEngine::Sprite* sprite) {
            if (sprite != NULL) {
                DrawSprite((UnityEngine::Texture*)sprite->get_texture(), indexToUse, glyph, assetToUse);
            } else {
                ScoreSaberLogger.info("{}", (WebUtils::API_URL + "unicode/" + utf8ToHex(unicode) + ".png").c_str());
            }
            
            loadingCount--;
            if (loadingCount == 0) {
                for (auto const& i : textToUpdate) {
                    i->ForceMeshUpdate(false, false);
                }
                textToUpdate = {};
            }
            
        }, true);

        currentEmojiIndex++;
        result = GetSpriteCharacterFromSpriteAsset(unicode, spriteAsset, includeFallbacks);
    }

    if (spriteAsset == currentEmojiAsset && loadingCount > 0) {
        textToUpdate.push_back(lastText);
    }

    return result;
}

MAKE_HOOK_MATCH(SetArraySizes, &TMPro::TextMeshProUGUI::SetArraySizes, int, TMPro::TextMeshProUGUI* self, ::ArrayW<::TMPro::TMP_Text::UnicodeChar> chars) {
    lastText = self; 
    
    return SetArraySizes(self, chars);
}

void EmojiSupport::AddSupport(TMPro::TextMeshProUGUI* text) {
    if (rootEmojiAsset == NULL) {
        clearPixels = ArrayW<UnityEngine::Color>(SHEET_SIZE * SHEET_SIZE);
        for (size_t i = 0; i < SHEET_SIZE * SHEET_SIZE; i++) {
            clearPixels[i] = UnityEngine::Color(0, 0, 0, 0);
        }
        
        rootEmojiAsset = CreateTMP_SpriteAsset();
        currentEmojiAsset = rootEmojiAsset;
        currentEmojiIndex = 0;
    }
    if (!hooksInstalled) {
        INSTALL_HOOK(ScoreSaberLogger, GetSpriteCharacterFromSpriteAsset);
        INSTALL_HOOK(ScoreSaberLogger, SetArraySizes);

        hooksInstalled = true;
    }

    text->m_spriteAsset = rootEmojiAsset;
}

void EmojiSupport::RemoveSupport(TMPro::TextMeshProUGUI* text) {
    text->set_spriteAsset(NULL);
}

void EmojiSupport::Reset() {
    rootEmojiAsset = NULL;
}