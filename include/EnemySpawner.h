#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Enemy.h"

class EnemySpawner {
public:
    EnemySpawner(int windowWidth, int windowHeight);

    // Call every frame - spawns enemies on a timer
    void update(float dt, std::vector<Enemy>& enemies);

    void setSpawnInterval(float seconds);  // how often a new enemy spawns

private:
    int   windowWidth;
    int   windowHeight;
    float spawnInterval;
    float spawnTimer = 0.f;

    // Returns a random position on one of the four screen edges
    sf::Vector2f randomEdgePosition();

    // Returns a random enemy type
    EnemyType randomEnemyType();
};