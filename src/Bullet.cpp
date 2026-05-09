#include "Bullet.h"
#include <cmath>

Bullet::Bullet(sf::Vector2f pos, sf::Vector2f velocity, float damage)
    : position(pos)
    , velocity(velocity)
    , damage(damage)
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

void         Bullet::expire()          { expired = true; }
bool         Bullet::isExpired() const { return expired; }
sf::Vector2f Bullet::getPosition()const { return position; }
float        Bullet::getDamage() const { return damage; }
