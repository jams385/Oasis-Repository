#include "Bullet.h"
#include <cmath>

static constexpr float PI = 3.14159265f;

Bullet::Bullet(sf::Vector2f pos, sf::Vector2f velocity, BulletConfig config)
    : position(pos)
    , velocity(velocity)
    , damage(config.damage)
    , aoeRadius(config.aoeRadius)
    , slowFactor(config.slowFactor)
    , slowDuration(config.slowDuration)
    , shapeType(config.shape)
{
    if (shapeType == BulletShape::Circle) {
        circleShape.setRadius(3.f);
        circleShape.setOrigin(3.f, 3.f);
        circleShape.setFillColor(sf::Color(100, 220, 255));
        circleShape.setPosition(position);
    } else {
        // Beam: a thin rectangle rotated to face the direction of travel
        beamShape.setSize({16.f, 4.f});
        beamShape.setOrigin(8.f, 2.f);
        beamShape.setFillColor(sf::Color(255, 220, 50));
        float angle = std::atan2(velocity.y, velocity.x) * 180.f / PI;
        beamShape.setRotation(angle);
        beamShape.setPosition(position);
    }
}

void Bullet::update(float dt) {
    if (expired) return;

    sf::Vector2f move = velocity * dt;
    position += move;
    traveled += std::sqrt(move.x*move.x + move.y*move.y);

    if (shapeType == BulletShape::Circle) circleShape.setPosition(position);
    else                                  beamShape.setPosition(position);

    if (traveled >= MAX_RANGE) expired = true;
}

void Bullet::draw(sf::RenderWindow& window) {
    if (expired) return;
    if (shapeType == BulletShape::Circle) window.draw(circleShape);
    else                                  window.draw(beamShape);
}

void         Bullet::expire()              { expired = true; }
bool         Bullet::isExpired()     const { return expired; }
sf::Vector2f Bullet::getPosition()   const { return position; }
float        Bullet::getDamage()     const { return damage; }
float        Bullet::getAoeRadius()  const { return aoeRadius; }
float        Bullet::getSlowFactor() const { return slowFactor; }
float        Bullet::getSlowDuration() const { return slowDuration; }
