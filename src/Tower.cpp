#include "Tower.h"
#include "AudioManager.h"
#include "GameUtils.h"
#include <iostream>

Tower::Tower(sf::Vector2f worldPos, TowerType type)
    : type(type)
    , position(worldPos)
{
    // shape.setRadius(10.f);
    // shape.setOrigin(10.f, 10.f);
    // shape.setOutlineThickness(2.f);
    // shape.setPosition(position);

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
            damage        = 5.f;
            burstSize     = 3;
            burstDelay    = 0.12f;
            burstCooldown = 2.0f;
            maxHp         = 150.f;
            
                texture.loadFromFile("C:/Users/CHENILLE ZYNE/Oasis-Repository/assets/OASIS-GRAPHICS/WATER_TOWER_2.png");
                std::cout << texture.getSize().x
          << " x "
          << texture.getSize().y
          << std::endl;
                sprite.setTexture(texture);
                sprite.setOrigin(
                    texture.getSize().x / 2.f,
                    texture.getSize().y / 2.f
                );
                sprite.setPosition(position);
                sprite.setScale(0.01f, 0.01f);

            // waterTowerTexture.loadFromFile("assets/OASIS-GRAPHICS/WATER_TOWER_2.png");
            // waterTowerSprite.setTexture(waterTowerTexture);
            // waterTowerSprite.setOrigin(
            //     waterTowerTexture.getSize().x / 2.f,
            //     waterTowerTexture.getSize().y / 2.f
            // );
            // waterTowerSprite.setPosition(position);
            // waterTowerSprite.setScale(0.22f, 0.22f);
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
            
                texture.loadFromFile("assets/OASIS-GRAPHICS/SUN_BEAM_TOWER_2.png");
                sprite.setTexture(texture);
                sprite.setOrigin(
                    texture.getSize().x / 2.f,
                    texture.getSize().y / 2.f
                );
                sprite.setPosition(position);
                sprite.setScale(0.22f, 0.22f);

            // sunBeamTexture.loadFromFile("assets/OASIS-GRAPHICS/SUN_BEAM_TOWER_2.png");
            // sunBeamSprite.setTexture(sunBeamTexture);
            // sunBeamSprite.setOrigin(
            //     sunBeamTexture.getSize().x / 2.f,
            //     sunBeamTexture.getSize().y / 2.f
            // );
            // sunBeamSprite.setPosition(position);
            // sunBeamSprite.setScale(0.22f, 0.22f);             // 260x500
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

                texture.loadFromFile("assets/OASIS-GRAPHICS/VINE_BURST.png");
                sprite.setTexture(texture);
                sprite.setOrigin(
                    texture.getSize().x / 2.f,
                    texture.getSize().y / 2.f
                );
                sprite.setPosition(position);
                sprite.setScale(0.42f, 0.42f);

            // treeTowerTexture.loadFromFile("assets/OASIS-GRAPHICS/VINE_BURST.png");
            // treeTowerSprite.setTexture(treeTowerTexture);
            // treeTowerSprite.setOrigin(
            //     treeTowerTexture.getSize().x / 2.f,
            //     treeTowerTexture.getSize().y / 2.f
            // );
            // treeTowerSprite.setPosition(position);
            // treeTowerSprite.setScale(1.f, 1.f);            // 150x160
            break;

        case TowerType::WaterMine:
            texture.loadFromFile("assets/OASIS-GRAPHICS/WATER_WELL.png");
            sprite.setTexture(texture);
            sprite.setOrigin(
                texture.getSize().x / 2.f,
                texture.getSize().y / 2.f
            );
            sprite.setPosition(position);
            sprite.setScale(0.35f, 0.35f);

        // waterMineTexture.loadFromFile("assets/OASIS-GRAPHICS/WATER_WELL.png");
        // waterMineSprite.setTexture(waterMineTexture);
        // waterMineSprite.setOrigin(
        //     waterMineTexture.getSize().x / 2.f,
        //     waterMineTexture.getSize().y / 2.f
        // );
        // waterMineSprite.setPosition(position);
        // waterMineSprite.setScale(0.35f, 0.35f);         // 170x200
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
            AudioManager::get().play("water_tower_shot", 15.f);
        else if (type == TowerType::SunBeam)
            AudioManager::get().play("sunbeam_shot", 30.f);
        else if (type == TowerType::TreeTower)
            AudioManager::get().play("tree_tower_shot", 50.f);
            
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
    sf::FloatRect bounds = sprite.getGlobalBounds();
    drawHealthBar(
        window,
        position.x - bounds.width / 2.f,
        position.y - bounds.height / 2.f - 10.f,
        bounds.width,
        5.f,
        hp / maxHp,
        sf::Color(255, 100, 50)
    );
}

void Tower::draw(sf::RenderWindow& window) {
    
    if (isDestroyed()) return;
    if (showRange) window.draw(rangeCircle);
    window.draw(sprite);
    drawHpBar(window);

    // switch (type)
    //     {
    //         case TowerType::WaterTower:
    //             window.draw(waterTowerSprite);
    //             break;
    //         case TowerType::SunBeam:
    //             window.draw(sunBeamSprite);
    //             break;
    //         case TowerType::TreeTower:
    //             window.draw(treeTowerSprite);
    //             break;
    //         case TowerType::WaterMine:
    //             window.draw(waterMineSprite);
    //             break;
    //     }

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
