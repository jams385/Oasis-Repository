#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Enemy.h"

class EnemySpawner {
public:
    EnemySpawner(int windowWidth, int windowHeight);

    // Call every frame - spawns enemies on a timer
    void update(float dt, std::vector<Enemy>& enemies, int waveNumber);

    void setSpawnInterval(float seconds);

private:
    int   windowWidth;
    int   windowHeight;
    float spawnInterval;
    float spawnTimer = 0.f;

    sf::Vector2f randomEdgePosition();
    EnemyType    randomEnemyType(int waveNumber);
};