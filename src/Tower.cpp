#include "Tower.h"
#include <cmath>

Tower::Tower(sf::Vector2f worldPos)
    : position(worldPos)
{
    shape.setRadius(14.f);
    shape.setOrigin(14.f, 14.f);
    shape.setFillColor(sf::Color(30, 100, 200));
    shape.setOutlineColor(sf::Color(100, 180, 255));
    shape.setOutlineThickness(2.f);
    shape.setPosition(position);

    rangeCircle.setRadius(range);
    rangeCircle.setOrigin(range, range);
    rangeCircle.setFillColor(sf::Color(100, 180, 255, 25));    // nearly transparent fill
    rangeCircle.setOutlineColor(sf::Color(100, 180, 255, 80)); // faint outline
    rangeCircle.setOutlineThickness(1.f);
    rangeCircle.setPosition(position);
}

void Tower::update(float dt, const std::vector<Enemy>& enemies, std::vector<Bullet>& bullets) {
    if (fireTimer > 0.f) fireTimer -= dt;

    // Find the nearest alive enemy within range
    const Enemy* target  = nullptr;
    float        minDist = range;

    for (const auto& e : enemies) {
        if (!e.isAlive()) continue;
        sf::Vector2f d    = e.getPosition() - position;
        float        dist = std::sqrt(d.x*d.x + d.y*d.y);
        if (dist < minDist) { minDist = dist; target = &e; }
    }

    // Fire when a target is found and the cooldown is ready
    if (target && fireTimer <= 0.f) {
        sf::Vector2f dir = target->getPosition() - position;
        float        len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        dir /= len;
        bullets.emplace_back(position, dir * 250.f, damage);
        fireTimer = 1.f / fireRate;
    }
}

void Tower::draw(sf::RenderWindow& window) {
    window.draw(rangeCircle);
    window.draw(shape);
}

sf::Vector2f Tower::getPosition() const { return position; }