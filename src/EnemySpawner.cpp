#include "EnemySpawner.h"
#include <cstdlib>

EnemySpawner::EnemySpawner(int windowWidth, int windowHeight){
    this->windowWidth  = windowWidth;
    this->windowHeight = windowHeight;
    this->spawnInterval = 3.f;
}

void EnemySpawner::update(float dt, std::vector<Enemy>& enemies) {
    spawnTimer += dt;

    if (spawnTimer >= spawnInterval) {
        sf::Vector2f spawnPos = randomEdgePosition();
        EnemyType    type     = randomEnemyType();
        enemies.emplace_back(type, spawnPos);
        spawnTimer = 0.f;
    }
}

void EnemySpawner::setSpawnInterval(float seconds) {
    spawnInterval = seconds;
}

// ── Random edge position ──────────────────────────────────────────────────────
sf::Vector2f EnemySpawner::randomEdgePosition() {
    // Pick one of 4 edges: 0=top, 1=bottom, 2=left, 3=right
    int edge = rand() % 4;

    switch (edge) {
        case 0:  // top edge
            return { (float)(rand() % windowWidth), -20.f };

        case 1:  // bottom edge
            return { (float)(rand() % windowWidth), (float)windowHeight + 20.f };

        case 2:  // left edge
            return { -20.f, (float)(rand() % windowHeight) };

        default: // right edge
            return { (float)windowWidth + 20.f, (float)(rand() % windowHeight) };
    }
}

// ── Random enemy type ─────────────────────────────────────────────────────────
EnemyType EnemySpawner::randomEnemyType() {
    int r = rand() % 3;
    switch (r) {
        case 0:  return EnemyType::DustMummy;
        case 1:  return EnemyType::SporePuff;
        default: return EnemyType::ShadowCrow;
    }
}