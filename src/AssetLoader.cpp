#include "AssetLoader.h"
#include "SpriteManager.h"
#include "AudioManager.h"

void AssetLoader::loadAll() {
    loadSprites();
    loadSounds();
}

void AssetLoader::loadSprites() {
    static const std::string MUMMY = "assets/OASIS-GRAPHICS/BLOOD_GOBLIN_SPRITE/";
    SpriteManager::get().loadTexture("dust_mummy_0", MUMMY + "1.png");
    SpriteManager::get().loadTexture("dust_mummy_1", MUMMY + "2.png");
    SpriteManager::get().loadTexture("dust_mummy_2", MUMMY + "3.png");
    SpriteManager::get().loadTexture("dust_mummy_3", MUMMY + "4.png");
    SpriteManager::get().loadTexture("dust_mummy_4", MUMMY + "5.png");
    SpriteManager::get().loadTexture("dust_mummy_5", MUMMY + "6.png");
}

void AssetLoader::loadSounds() {
    AudioManager::get().loadSound("construction", "assets/audio/construction.wav");
    AudioManager::get().loadSound("flute_start",  "assets/audio/Flute_start.wav");
    AudioManager::get().loadSound("cutscene_sfx",  "assets/audio/OasisCutscene.ogg");

    AudioManager::get().playMusic("assets/audio/OasisTheme.ogg");
}

