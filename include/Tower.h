#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Enemy.h"
#include "Bullet.h"

enum class TowerType { WaterTower, SunBeam, TreeTower };

class Tower {
public:
    Tower(sf::Vector2f worldPos, TowerType type);

    void update(float dt, const std::vector<Enemy>& enemies, std::vector<Bullet>& bullets);
    void draw(sf::RenderWindow& window);

    sf::Vector2f getPosition() const;

private:
    TowerType       type;
    sf::Vector2f    position;

    /* ── EDIT TOWER STATS IN initStats() ───────────────────────────────────── */
    float           range         = 150.f;
    float           damage        = 10.f;
    float           fireTimer     = 0.f;

    // Burst fire
    int             burstSize      = 1;     // shots per burst
    int             burstShotsLeft = 0;
    float           burstDelay     = 0.f;   // seconds between shots within a burst
    float           burstTimer     = 0.f;
    float           burstCooldown  = 1.f;   // seconds between bursts

    sf::CircleShape shape;
    sf::CircleShape rangeCircle;

    void initStats();
};
