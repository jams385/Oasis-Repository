#pragma once
#include <SFML/Graphics.hpp>

struct BulletConfig {
    float damage       = 0.f;
    float aoeRadius    = 0.f;  // 0 = single target
    float slowFactor   = 1.f;  // 1 = no slow, 0.4 = 40% speed
    float slowDuration = 0.f;
};

class Bullet {
public:
    Bullet(sf::Vector2f pos, sf::Vector2f velocity, BulletConfig config);

    void update(float dt);
    void draw(sf::RenderWindow& window);

    void         expire();
    bool         isExpired()     const;
    sf::Vector2f getPosition()   const;
    float        getDamage()     const;
    float        getAoeRadius()  const;
    float        getSlowFactor() const;
    float        getSlowDuration() const;

private:
    sf::Vector2f    position;
    sf::Vector2f    velocity;
    float           damage;
    float           aoeRadius;
    float           slowFactor;
    float           slowDuration;
    bool            expired  = false;
    float           traveled = 0.f;
    static constexpr float MAX_RANGE = 300.f;

    sf::CircleShape shape;
};
