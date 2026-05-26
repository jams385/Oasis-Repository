#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Enemy.h"
#include "Bullet.h"

enum class TowerType { WaterTower, SunBeam, TreeTower, Cornucopia, WaterMine };

class Tower {
public:
    Tower(sf::Vector2f worldPos, TowerType type);

    void update(float dt, const std::vector<Enemy>& enemies, std::vector<Bullet>& bullets);
    void draw(sf::RenderWindow& window);

    sf::Vector2f getPosition() const;
    static int   getCost(TowerType type);

private:
    TowerType       type;
    sf::Vector2f    position;


    /* Defaults */
    float           range         = 150.f;
    float           damage        = 10.f;
    float           bulletSpeed   = 250.f;
    float           fireTimer     = 0.f; 

    /* Slowing Down */
    float           aoeRadius     = 0.f;
    float           slowFactor    = 1.f;
    float           slowDuration  = 0.f;

    BulletShape     bulletShape   = BulletShape::Circle;

    /* Burst Shots */
    int             burstSize      = 1;     
    int             burstShotsLeft = 0;
    float           burstDelay     = 0.f; 
    float           burstTimer     = 0.f;
    float           burstCooldown  = 1.f;  

    sf::CircleShape shape;
    sf::CircleShape rangeCircle;

    void initStats();
};
