#include "AssetLoader.h"
#include "SpriteManager.h"
#include "AudioManager.h"

void AssetLoader::loadAll() {
    loadSprites();
    loadSounds();
}

void AssetLoader::loadSprites() {
    
    // ── Dust Mummy  ──────────────────────────────────────────────────────────────────────
    static const std::string MUMMY = "assets/OASIS-GRAPHICS/2-BLOOD_GOBLIN_SPRITE/";
    SpriteManager::get().loadTexture("dust_mummy_0", MUMMY + "1.png");
    SpriteManager::get().loadTexture("dust_mummy_1", MUMMY + "2.png");
    SpriteManager::get().loadTexture("dust_mummy_2", MUMMY + "3.png");
    SpriteManager::get().loadTexture("dust_mummy_3", MUMMY + "4.png");
    SpriteManager::get().loadTexture("dust_mummy_4", MUMMY + "5.png");
    SpriteManager::get().loadTexture("dust_mummy_5", MUMMY + "6.png");

    // ── Rust Golem ───────────────────────────────────────────────────────────────
    static const std::string RUST_GOLEM = "assets/OASIS-GRAPHICS/2-RUST_GOLEM_SPRITE/";
    SpriteManager::get().loadTexture("rust_golem_0", RUST_GOLEM + "1.png");
    SpriteManager::get().loadTexture("rust_golem_1", RUST_GOLEM + "2.png");
    SpriteManager::get().loadTexture("rust_golem_2", RUST_GOLEM + "3.png");
    SpriteManager::get().loadTexture("rust_golem_3", RUST_GOLEM + "4.png");
    SpriteManager::get().loadTexture("rust_golem_4", RUST_GOLEM + "5.png");
    SpriteManager::get().loadTexture("rust_golem_5", RUST_GOLEM + "6.png");

    // ── Shadow Crow ──────────────────────────────────────────────────────────────
    static const std::string SHADOW_CROW = "assets/OASIS-GRAPHICS/2-SHADOW_CROW_SPRITE/";
    SpriteManager::get().loadTexture("shadow_crow_0", SHADOW_CROW + "1.png");
    SpriteManager::get().loadTexture("shadow_crow_1", SHADOW_CROW + "2.png");
    SpriteManager::get().loadTexture("shadow_crow_2", SHADOW_CROW + "3.png");
    SpriteManager::get().loadTexture("shadow_crow_3", SHADOW_CROW + "4.png");

    // ── Spore Puff ───────────────────────────────────────────────────────────────
    static const std::string SPORE_PUFF = "assets/OASIS-GRAPHICS/2-SPORE-PUFF-SPRITE/";
    SpriteManager::get().loadTexture("spore_puff_0", SPORE_PUFF + "1.png");
    SpriteManager::get().loadTexture("spore_puff_1", SPORE_PUFF + "2.png");
    SpriteManager::get().loadTexture("spore_puff_2", SPORE_PUFF + "3.png");
    SpriteManager::get().loadTexture("spore_puff_3", SPORE_PUFF + "4.png");
    SpriteManager::get().loadTexture("spore_puff_4", SPORE_PUFF + "5.png");
    SpriteManager::get().loadTexture("spore_puff_5", SPORE_PUFF + "6.png");

    // ── Soldier Attack ───────────────────────────────────────────────────────────
    static const std::string SOLDIER_ATTACK = "assets/OASIS-GRAPHICS/SOLDIER_ATTACK_SPRITE/";
    SpriteManager::get().loadTexture("soldier_attack_0", SOLDIER_ATTACK + "1.png");
    SpriteManager::get().loadTexture("soldier_attack_1", SOLDIER_ATTACK + "2.png");
    SpriteManager::get().loadTexture("soldier_attack_2", SOLDIER_ATTACK + "3.png");
    SpriteManager::get().loadTexture("soldier_attack_3", SOLDIER_ATTACK + "4.png");

    // ── Soldier Running ──────────────────────────────────────────────────────────
    static const std::string SOLDIER_RUNNING = "assets/OASIS-GRAPHICS/SOLDIER_RUNNING_SPRITE/";
    SpriteManager::get().loadTexture("soldier_running_0", SOLDIER_RUNNING + "1.png");
    SpriteManager::get().loadTexture("soldier_running_1", SOLDIER_RUNNING + "2.png");
    SpriteManager::get().loadTexture("soldier_running_2", SOLDIER_RUNNING + "3.png");
    SpriteManager::get().loadTexture("soldier_running_3", SOLDIER_RUNNING + "4.png");
    SpriteManager::get().loadTexture("soldier_running_4", SOLDIER_RUNNING + "5.png");
    SpriteManager::get().loadTexture("soldier_running_5", SOLDIER_RUNNING + "6.png");
    SpriteManager::get().loadTexture("soldier_running_6", SOLDIER_RUNNING + "7.png");
    SpriteManager::get().loadTexture("soldier_running_7", SOLDIER_RUNNING + "8.png");
}

void AssetLoader::loadSounds() {
    AudioManager::get().loadSound("construction", "assets/audio/construction.wav");
    AudioManager::get().loadSound("flute_start",  "assets/audio/Flute_start.wav");
    AudioManager::get().loadSound("WaterMine_Collect",  "assets/audio/WaterMine_Collect.wav");
    AudioManager::get().loadSound("WaterMine_Collect_25",  "assets/audio/WaterMine_Collect2.wav");
    AudioManager::get().loadSound("water_tower_shot", "assets/audio/WaterTower_Shot.wav");
    AudioManager::get().loadSound("sunbeam_shot",     "assets/audio/sunbeam.wav");
    AudioManager::get().loadSound("tree_tower_shot",  "assets/audio/TreeTower.wav");
    AudioManager::get().loadSound("click_tower",      "assets/audio/ClickTower.wav");
    AudioManager::get().loadSound("win_sound",        "assets/audio/WinSound.wav");
    AudioManager::get().loadSound("lose_sound",        "assets/audio/LoseSound.wav");

    AudioManager::get().playMusic("assets/audio/OasisTheme.ogg");
}

