#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Enemy.h"
#include "Bullet.h"

class Tower {
public:
    Tower(sf::Vector2f worldPos);

    void update(float dt, const std::vector<Enemy>& enemies, std::vector<Bullet>& bullets);
    void draw(sf::RenderWindow& window);

    sf::Vector2f getPosition() const;

private:
    sf::Vector2f    position;

    /* ── EDIT TOWER STATS HERE ──────────────────────────────────────────────
       range    = how far the tower can see (pixels)
       damage   = HP removed from enemy per bullet
       fireRate = bullets per second                                         */
    float           range     = 150.f;
    float           damage    = 20.f;
    float           fireRate  = 1.0f;
    float           fireTimer = 0.f;

    sf::CircleShape shape;
    sf::CircleShape rangeCircle;
};
