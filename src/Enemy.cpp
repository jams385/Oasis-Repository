#include "Enemy.h"
#include <cmath>


static const float ATTACK_RANGE = 24.f;

Enemy::Enemy(EnemyType type, sf::Vector2f startPos)
    : type(type)
    , position(startPos)
    , attackTimer(0.f)
    , attacking(false)
    , alive(true)
{
    initStats();
    shape.setOrigin(shape.getRadius(), shape.getRadius());
    shape.setPosition(position);
}

/* ── EDIT ENEMY STATS HERE ─────────────────────────────────────────────────
   damage      = HP removed from cornucopia per hit
   attackSpeed = hits per second (e.g. 1.0 = once/sec, 2.0 = twice/sec)    */
void Enemy::initStats() {
    switch (type) {

        case EnemyType::DustMummy:
            hp          = 100.f;
            speed       = 60.f;
            damage      = 15.f;
            attackSpeed = 0.8f;
            reward      = 10;
            shape.setRadius(14.f);
            shape.setFillColor(sf::Color(210, 180, 140));
            shape.setOutlineColor(sf::Color(120, 90, 50));
            shape.setOutlineThickness(2.f);
            break;

        case EnemyType::SporePuff:
            hp          = 40.f;
            speed       = 120.f;
            damage      = 8.f;
            attackSpeed = 1.2f;
            reward      = 15;
            shape.setRadius(10.f);
            shape.setFillColor(sf::Color(180, 130, 200));
            shape.setOutlineColor(sf::Color(120, 60, 160));
            shape.setOutlineThickness(2.f);
            break;

        case EnemyType::ShadowCrow:
            hp          = 25.f;
            speed       = 200.f;
            damage      = 5.f;
            attackSpeed = 1.5f;
            reward      = 20;
            shape.setRadius(8.f);
            shape.setFillColor(sf::Color(50, 50, 80));
            shape.setOutlineColor(sf::Color(100, 100, 180));
            shape.setOutlineThickness(1.5f);
            break;
    }

    maxHp = hp;
}


void Enemy::update(float dt, sf::Vector2f target) {
    if (!alive) return;

    sf::Vector2f dir  = target - position;
    float        dist = std::sqrt(dir.x*dir.x + dir.y*dir.y);

    if (dist <= ATTACK_RANGE) {
        // In melee range — stand still and tick the attack cooldown
        attacking = true;
        if (attackTimer > 0.f) attackTimer -= dt;
    } else {
        // Move toward target
        attacking   = false;
        attackTimer = 0.f;    // reset so first hit lands the moment enemy arrives
        dir        /= dist;
        position   += dir * speed * dt;
        shape.setPosition(position);
    }
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

bool Enemy::consumeAttack() {
    if (!attacking || attackTimer > 0.f) return false;
    attackTimer = 1.f / attackSpeed;   // set cooldown for next swing
    return true;
}

bool         Enemy::isAlive()     const { return alive; }
bool         Enemy::isAttacking() const { return attacking; }
sf::Vector2f Enemy::getPosition() const { return position; }
float        Enemy::getDamage()   const { return damage; }
int          Enemy::getReward()   const { return reward; }
EnemyType    Enemy::getType()     const { return type; }