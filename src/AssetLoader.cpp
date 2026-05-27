#include "AssetLoader.h"
#include "SpriteManager.h"
#include "AudioManager.h"

void AssetLoader::loadAll() {
    loadSprites();
    loadSounds();
}

void AssetLoader::loadSprites() {
    static const std::string MUMMY = "assets/OASIS-GRAPHICS/little mummy walk/";
    SpriteManager::get().loadTexture("dust_mummy_0", MUMMY + "sprite_0.png");
    SpriteManager::get().loadTexture("dust_mummy_1", MUMMY + "sprite_1.png");
    SpriteManager::get().loadTexture("dust_mummy_2", MUMMY + "sprite_2.png");
    SpriteManager::get().loadTexture("dust_mummy_3", MUMMY + "sprite_3.png");
    SpriteManager::get().loadTexture("dust_mummy_4", MUMMY + "sprite_4.png");
    SpriteManager::get().loadTexture("dust_mummy_5", MUMMY + "sprite_5.png");
}

void AssetLoader::loadSounds() {
    AudioManager::get().loadSound("construction", "assets/audio/construction.wav");
    AudioManager::get().loadSound("flute_start",  "assets/audio/Flute_start.wav");
    AudioManager::get().loadSound("cutscene_sfx",  "assets/audio/OasisCutscene.ogg");

    AudioManager::get().playMusic("assets/audio/OasisTheme.ogg");
}

