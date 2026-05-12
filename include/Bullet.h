#pragma once
#include <SFML/Graphics.hpp>

enum class BulletShape { Circle, Beam };

struct BulletConfig {
    float       damage       = 0.f;
    float       aoeRadius    = 0.f;
    float       slowFactor   = 1.f;
    float       slowDuration = 0.f;
    BulletShape shape        = BulletShape::Circle;
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
    sf::Vector2f       position;
    sf::Vector2f       velocity;
    float              damage;
    float              aoeRadius;
    float              slowFactor;
    float              slowDuration;
    BulletShape        shapeType;
    bool               expired  = false;
    float              traveled = 0.f;
    static constexpr float MAX_RANGE = 300.f;

    sf::CircleShape    circleShape;
    sf::RectangleShape beamShape;
};
