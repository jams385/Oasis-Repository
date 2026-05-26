#include "Tower.h"
#include <cmath>

Tower::Tower(sf::Vector2f worldPos, TowerType type)
    : type(type)
    , position(worldPos)
{
    shape.setRadius(14.f);
    shape.setOrigin(14.f, 14.f);
    shape.setOutlineThickness(2.f);
    shape.setPosition(position);

    initStats();

    rangeCircle.setRadius(range);
    rangeCircle.setOrigin(range, range);
    rangeCircle.setFillColor(sf::Color(100, 180, 255, 25));
    rangeCircle.setOutlineColor(sf::Color(100, 180, 255, 80));
    rangeCircle.setOutlineThickness(1.f);
    rangeCircle.setPosition(position);
}

/* ----------------------------------------------
    STAT EDITOR
------------------------------------------------- */
void Tower::initStats() {
    switch (type) {

        case TowerType::WaterTower:
            range         = 150.f;
            damage        = 8.f;
            burstSize     = 3;
            burstDelay    = 0.12f;
            burstCooldown = 2.0f;
            maxHp         = 150.f;
            shape.setFillColor(sf::Color(30, 100, 200));
            shape.setOutlineColor(sf::Color(100, 180, 255));
            break;

        case TowerType::SunBeam:
            range         = 250.f;
            damage        = 45.f;
            bulletSpeed   = 350.f;
            bulletShape   = BulletShape::Beam;
            burstSize     = 1;
            burstDelay    = 0.f;
            burstCooldown = 3.0f;
            maxHp         = 200.f;
            shape.setFillColor(sf::Color(220, 180, 30));
            shape.setOutlineColor(sf::Color(255, 230, 100));
            break;

        case TowerType::TreeTower:
            range         = 130.f;
            damage        = 10.f;
            bulletSpeed   = 200.f;
            aoeRadius     = 60.f;
            slowFactor    = 0.4f;
            slowDuration  = 3.0f;
            burstSize     = 1;
            burstDelay    = 0.f;
            burstCooldown = 2.0f;
            maxHp         = 120.f;
            shape.setFillColor(sf::Color(34, 120, 34));
            shape.setOutlineColor(sf::Color(80, 180, 80));
            break;

        case TowerType::Cornucopia:
        case TowerType::WaterMine:
            break;
    }

    hp = maxHp;
}

void Tower::update(float dt, const std::vector<Enemy>& enemies, std::vector<Bullet>& bullets) {
    if (isDestroyed()) return;

    if (fireTimer  > 0.f) fireTimer  -= dt;
    if (burstTimer > 0.f) burstTimer -= dt;

    // Find the nearest alive enemy within range
    const Enemy* target  = nullptr;
    float        minDist = range;

    for (const auto& e : enemies) {
        if (!e.isAlive()) continue;
        sf::Vector2f d    = e.getPosition() - position;
        float        dist = std::sqrt(d.x*d.x + d.y*d.y);
        if (dist < minDist) { minDist = dist; target = &e; }
    }

    // Start a new burst when cooldown is ready and a target is in range
    if (burstShotsLeft == 0 && fireTimer <= 0.f && target) {
        burstShotsLeft = burstSize;
        burstTimer     = 0.f;
        fireTimer      = burstCooldown;
    }

    // Fire one shot per burstTimer tick while mid-burst
    if (burstShotsLeft > 0 && burstTimer <= 0.f && target) {
        sf::Vector2f dir = target->getPosition() - position;
        float        len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        dir /= len;
        BulletConfig cfg;
        cfg.damage       = damage;
        cfg.aoeRadius    = aoeRadius;
        cfg.slowFactor   = slowFactor;
        cfg.slowDuration = slowDuration;
        cfg.shape        = bulletShape;
        bullets.emplace_back(position, dir * bulletSpeed, cfg);
        burstShotsLeft--;
        burstTimer = burstDelay;
    }
}

void Tower::takeDamage(float amount) {
    hp -= amount;
    if (hp <= 0.f) hp = 0.f;
}

bool Tower::isDestroyed() const { return hp <= 0.f; }

void Tower::drawHpBar(sf::RenderWindow& window) {
    float radius   = shape.getRadius();
    float barWidth = radius * 2.f;
    float ratio    = hp / maxHp;

    sf::RectangleShape bg({ barWidth, 4.f });
    bg.setFillColor(sf::Color(80, 0, 0));
    bg.setPosition(position.x - radius, position.y - radius - 6.f);
    window.draw(bg);

    sf::RectangleShape bar({ barWidth * ratio, 4.f });
    bar.setFillColor(sf::Color(255, 100, 50));
    bar.setPosition(position.x - radius, position.y - radius - 6.f);
    window.draw(bar);
}

void Tower::draw(sf::RenderWindow& window) {
    if (isDestroyed()) return;
    window.draw(rangeCircle);
    window.draw(shape);
    drawHpBar(window);
}

sf::Vector2f Tower::getPosition() const { return position; }

int Tower::getCost(TowerType type) {
    switch (type) {
        case TowerType::WaterTower: return 50;
        case TowerType::SunBeam:    return 100;
        case TowerType::TreeTower:  return 75;
        case TowerType::Cornucopia: return 200;
        case TowerType::WaterMine:  return 60;
        default:                    return 0;
    }
}
