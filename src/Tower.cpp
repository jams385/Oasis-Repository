#include "Tower.h"
#include "AudioManager.h"
#include "GameUtils.h"

Tower::Tower(sf::Vector2f worldPos, TowerType type)
    : type(type)
    , position(worldPos)
{
    shape.setRadius(10.f);
    shape.setOrigin(10.f, 10.f);
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
            damage        = 7.f;
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

        case TowerType::WaterMine:
            break;
    }

    hp = maxHp;
}

void Tower::update(float dt, const std::vector<Enemy>& enemies, std::vector<Bullet>& bullets) {
    if (isDestroyed()) return;

    timeSinceDamage += dt;
    if (timeSinceDamage >= HEAL_DELAY && hp < maxHp) {
        hp += HEAL_RATE * dt;
        if (hp > maxHp) hp = maxHp;
    }

    if (fireTimer  > 0.f) fireTimer  -= dt;
    if (burstTimer > 0.f) burstTimer -= dt;

    // Find the nearest alive enemy within range
    const Enemy* target  = nullptr;
    float        minDist = range;

    for (const auto& e : enemies) {
        if (!e.isAlive()) continue;
        float d = dist(e.getPosition(), position);
        if (d < minDist) { minDist = d; target = &e; }
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
        dir /= dist(target->getPosition(), position);
        BulletConfig cfg;
        cfg.damage       = damage;
        cfg.aoeRadius    = aoeRadius;
        cfg.slowFactor   = slowFactor;
        cfg.slowDuration = slowDuration;
        cfg.shape        = bulletShape;
        bullets.emplace_back(position, dir * bulletSpeed, cfg);
        if (type == TowerType::WaterTower)
            AudioManager::get().play("water_tower_shot", 60.f);
        burstShotsLeft--;
        burstTimer = burstDelay;
    }
}

void Tower::takeDamage(float amount) {
    hp -= amount;
    if (hp <= 0.f) hp = 0.f;
    timeSinceDamage = 0.f;
}

bool Tower::isDestroyed() const { return hp <= 0.f; }

void Tower::drawHpBar(sf::RenderWindow& window) {
    float r = shape.getRadius();
    drawHealthBar(window, position.x - r, position.y - r - 6.f,
                  r * 2.f, 4.f, hp / maxHp, sf::Color(255, 100, 50));
}

void Tower::draw(sf::RenderWindow& window) {
    if (isDestroyed()) return;
    if (showRange) window.draw(rangeCircle);
    window.draw(shape);
    drawHpBar(window);
}

void Tower::setShowRange(bool show) { showRange = show; }

sf::Vector2f Tower::getPosition() const { return position; }
TowerType    Tower::getType()     const { return type; }

int Tower::getCost(TowerType type) {
    switch (type) {
        case TowerType::WaterTower: return 60;
        case TowerType::SunBeam:    return 150;
        case TowerType::TreeTower:  return 75;
        case TowerType::WaterMine:  return 60;
        default:                    return 0;
    }
}
