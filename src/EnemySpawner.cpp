#include "EnemySpawner.h"
#include <cstdlib>
#include <vector>

EnemySpawner::EnemySpawner(int windowWidth, int windowHeight){
    this->windowWidth  = windowWidth;
    this->windowHeight = windowHeight;
    this->spawnInterval = 3.f;
}

void EnemySpawner::update(float dt, std::vector<Enemy>& enemies, int waveNumber) {
    spawnTimer += dt;

    if (spawnTimer >= spawnInterval) {
        sf::Vector2f spawnPos = randomEdgePosition();
        EnemyType    type     = randomEnemyType(waveNumber);
        enemies.emplace_back(type, spawnPos);
        spawnTimer = 0.f;
    }
}

void EnemySpawner::setSpawnInterval(float seconds) {
    spawnInterval = seconds;
}

// ── Random edge position ──────────────────────────────────────────────────────
sf::Vector2f EnemySpawner::randomEdgePosition() {
    int edge = rand() % 4;

    switch (edge) {
        case 0:  return { (float)(rand() % windowWidth), -20.f };
        case 1:  return { (float)(rand() % windowWidth), (float)windowHeight + 20.f };
        case 2:  return { -20.f, (float)(rand() % windowHeight) };
        default: return { (float)windowWidth + 20.f, (float)(rand() % windowHeight) };
    }
}

// ── Wave-gated enemy pool ─────────────────────────────────────────────────────
// wave  1-5 : DustMummy only
// wave  6-10: + ShadowCrow
// wave 11-15: + SporePuff
// wave   16+: + RustGolem
EnemyType EnemySpawner::randomEnemyType(int waveNumber) {
    std::vector<EnemyType> pool;

    pool.push_back(EnemyType::DustMummy);

    if (waveNumber >= 6)  pool.push_back(EnemyType::ShadowCrow);
    if (waveNumber >= 11) pool.push_back(EnemyType::SporePuff);
    if (waveNumber >= 16) pool.push_back(EnemyType::RustGolem);

    return pool[rand() % pool.size()];
}
