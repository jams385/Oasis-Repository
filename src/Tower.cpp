#include "Tower.h"
#include <cmath>

//a function to find the distance of 2 points
static float distance(sf::Vector2f a, sf::Vector2f b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return std::sqrt(dx*dx + dy*dy);
}

//initializes the tower
Tower::Tower(sf::Vector2f pos, float range, float fireRate, float damage)
    : position(pos), range(range), fireRate(fireRate), damage(damage), cooldown(0.f)
{
    // Tower shape - blue circle
    shape.setRadius(20.f);
    shape.setOrigin(20.f, 20.f);
    shape.setFillColor(sf::Color(30, 100, 200));
    shape.setOutlineColor(sf::Color(100, 180, 255));
    shape.setOutlineThickness(3.f);
    shape.setPosition(position);

    // Range indicator
    rangeCircle.setRadius(range);
    rangeCircle.setOrigin(range, range);
    rangeCircle.setFillColor(sf::Color(100, 180, 255, 20));
    rangeCircle.setOutlineColor(sf::Color(100, 180, 255, 80));
    rangeCircle.setOutlineThickness(1.f);
    rangeCircle.setPosition(position);
}


void Tower::update(float dt, std::vector<Enemy>& enemies) {

    // Tick cooldown down
    cooldown -= dt;
    if (cooldown < 0.f) cooldown = 0.f;

    // Move all existing bullets
    for (auto& b : bullets) {
        b.pos += b.vel * dt;

        // Kill bullet if it goes off screen
        if (b.pos.x < 0 || b.pos.x > 1280 ||
            b.pos.y < 0 || b.pos.y > 720)
            b.alive = false;

        // Check if bullet hits an enemy
        for (auto& e : enemies) {
            if (!e.isAlive()) continue;
            if (distance(b.pos, e.getPosition()) < 22.f) {
                e.takeDamage(damage);
                b.alive = false;
                break;
            }
        }
    }

    // Remove dead bullets
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b){ return !b.alive; }),
        bullets.end()
    );

    // Fire at nearest enemy if cooldown is ready
    if (cooldown > 0.f) return;

    Enemy* target = findTarget(enemies);
    if (!target) return;

    // Build a bullet aimed at the target
    sf::Vector2f dir = target->getPosition() - position;
    float dist = distance(position, target->getPosition());
    dir /= dist;  // normalize

    Bullet b;
    b.pos = position;
    b.vel = dir * 400.f;  // bullet speed pixels/sec
    bullets.push_back(b);

    cooldown = 1.f / fireRate;  // reset cooldown
}

// draws the tower
void Tower::draw(sf::RenderWindow& window) {
    window.draw(rangeCircle);
    window.draw(shape);

    // Draw all bullets
    for (auto& b : bullets) {
        if (!b.alive) continue;
        sf::CircleShape bs(5.f);
        bs.setOrigin(5.f, 5.f);
        bs.setFillColor(sf::Color(0, 191, 255));
        bs.setPosition(b.pos);
        window.draw(bs);
    }
}

//targets enemies in range
Enemy* Tower::findTarget(std::vector<Enemy>& enemies) {
    Enemy* nearest = nullptr;
    float  minDist = range;  // only consider enemies within range

    for (auto& e : enemies) {
        if (!e.isAlive()) continue;
        float dist = distance(position, e.getPosition());
        if (dist < minDist) {
            minDist = dist;
            nearest = &e;
        }
    }
    return nearest;
}