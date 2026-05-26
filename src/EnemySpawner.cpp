#include "EnemySpawner.h"
#include <algorithm>
#include <cstdlib>
#include <vector>

EnemySpawner::EnemySpawner(int windowWidth, int windowHeight){
    this->windowWidth  = windowWidth;
    this->windowHeight = windowHeight;
    this->spawnInterval = 3.f;
}

void EnemySpawner::update(float dt, std::vector<Enemy>& enemies, int waveNumber) {
    spawnTimer += dt;

    /* EDIT SPAWN INTERVALS HERE 
        max(limit, current interval)
    */
    float interval = std::max(0.5f, spawnInterval - (waveNumber - 1) * 0.25f);

    if (spawnTimer >= interval) {
        sf::Vector2f spawnPos = randomEdgePosition();
        EnemyType    type     = randomEnemyType(waveNumber);
        enemies.emplace_back(type, spawnPos);
        spawnTimer = 0.f;
    }
}

void EnemySpawner::setSpawnInterval(float seconds) {
    spawnInterval = seconds;
}


sf::Vector2f EnemySpawner::randomEdgePosition() {
    int edge = rand() % 4;

    switch (edge) {
        case 0:  return { (float)(rand() % windowWidth), -20.f };
        case 1:  return { (float)(rand() % windowWidth), (float)windowHeight + 20.f };
        case 2:  return { -20.f, (float)(rand() % windowHeight) };
        default: return { (float)windowWidth + 20.f, (float)(rand() % windowHeight) };
    }
}

/* CUSTOMIZE ENEMY POOL PER WAVE */
EnemyType EnemySpawner::randomEnemyType(int waveNumber) {
    std::vector<EnemyType> pool;

    pool.push_back(EnemyType::DustMummy);

    if (waveNumber >= 2)  pool.push_back(EnemyType::ShadowCrow);
    if (waveNumber >= 3) pool.push_back(EnemyType::SporePuff);
    if (waveNumber >= 4) pool.push_back(EnemyType::RustGolem);

    return pool[rand() % pool.size()];
}
