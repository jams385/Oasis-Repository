#include "Enemy.h"
#include <cmath>


Enemy::Enemy(EnemyType type, sf::Vector2f startPos)
    : type(type)
    , position(startPos)
    , alive(true)
{
    initStats();
    shape.setOrigin(shape.getRadius(), shape.getRadius());
    shape.setPosition(position);
}

/* EDIT ENEMY STATS HERE!!! */
void Enemy::initStats() {
    switch (type) {

        case EnemyType::DustMummy:
            // Slow and tanky — basic enemy
            hp     = 100.f;
            speed  = 60.f;
            damage = 15.f;
            reward = 10;
            shape.setRadius(14.f);
            shape.setFillColor(sf::Color(210, 180, 140));   // tan
            shape.setOutlineColor(sf::Color(120, 90, 50));
            shape.setOutlineThickness(2.f);
            break;

        case EnemyType::SporePuff:
            // Fast and fragile
            hp     = 40.f;
            speed  = 120.f;
            damage = 8.f;
            reward = 15;
            shape.setRadius(10.f);
            shape.setFillColor(sf::Color(180, 130, 200));   // purple
            shape.setOutlineColor(sf::Color(120, 60, 160));
            shape.setOutlineThickness(2.f);
            break;

        case EnemyType::ShadowCrow:
            // Very fast, small, hard to hit
            hp     = 25.f;
            speed  = 200.f;
            damage = 5.f;
            reward = 20;
            shape.setRadius(8.f);
            shape.setFillColor(sf::Color(50, 50, 80));      // dark blue
            shape.setOutlineColor(sf::Color(100, 100, 180));
            shape.setOutlineThickness(1.5f);
            break;
    }

    maxHp = hp;
}


void Enemy::update(float dt, sf::Vector2f target) {
    if (!alive) return;

    // Move toward target
    sf::Vector2f dir  = target - position;
    float        dist = std::sqrt(dir.x*dir.x + dir.y*dir.y);

    if (dist > 1.f) {
        dir      /= dist;          // normalize
        position += dir * speed * dt;
    }

    shape.setPosition(position);
}


void Enemy::draw(sf::RenderWindow& window) {
    if (!alive) return;
    window.draw(shape);
    drawHpBar(window);
}


void Enemy::drawHpBar(sf::RenderWindow& window) {
    float radius  = shape.getRadius();
    float barWidth = radius * 2.f;
    float ratio   = hp / maxHp;

    sf::RectangleShape bg({ barWidth, 4.f });
    bg.setFillColor(sf::Color(80, 0, 0));
    bg.setPosition(position.x - radius, position.y - radius - 6.f);
    window.draw(bg);

    sf::RectangleShape bar({ barWidth * ratio, 4.f });
    bar.setFillColor(sf::Color(60, 200, 80));
    bar.setPosition(position.x - radius, position.y - radius - 6.f);
    window.draw(bar);
}


void Enemy::takeDamage(float amount) {
    hp -= amount;
    if (hp <= 0.f) { hp = 0.f; alive = false; }
}

// ── Checks ────────────────────────────────────────────────────────────────────
bool Enemy::isAlive() const { return alive; }

bool Enemy::hasReachedTarget(sf::Vector2f target) const {
    sf::Vector2f diff = target - position;
    float dist = std::sqrt(diff.x*diff.x + diff.y*diff.y);
    return dist < 20.f;
}

sf::Vector2f Enemy::getPosition() const { return position; }
float        Enemy::getDamage()   const { return damage; }
int          Enemy::getReward()   const { return reward; }
EnemyType    Enemy::getType()     const { return type; }