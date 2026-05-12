#include "Bullet.h"
#include <cmath>

Bullet::Bullet(sf::Vector2f pos, sf::Vector2f velocity, BulletConfig config)
    : position(pos)
    , velocity(velocity)
    , damage(config.damage)
    , aoeRadius(config.aoeRadius)
    , slowFactor(config.slowFactor)
    , slowDuration(config.slowDuration)
{
    shape.setRadius(4.f);
    shape.setOrigin(4.f, 4.f);
    shape.setFillColor(sf::Color(100, 220, 255));
    shape.setPosition(position);
}

void Bullet::update(float dt) {
    if (expired) return;

    sf::Vector2f move = velocity * dt;
    position += move;
    traveled += std::sqrt(move.x*move.x + move.y*move.y);
    shape.setPosition(position);

    if (traveled >= MAX_RANGE) expired = true;
}

void Bullet::draw(sf::RenderWindow& window) {
    if (!expired) window.draw(shape);
}

void         Bullet::expire()              { expired = true; }
bool         Bullet::isExpired()     const { return expired; }
sf::Vector2f Bullet::getPosition()   const { return position; }
float        Bullet::getDamage()     const { return damage; }
float        Bullet::getAoeRadius()  const { return aoeRadius; }
float        Bullet::getSlowFactor() const { return slowFactor; }
float        Bullet::getSlowDuration() const { return slowDuration; }
