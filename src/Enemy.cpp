#include "Enemy.h"
#include "AudioManager.h"
#include "GameUtils.h"


static const float ATTACK_RANGE = 18.f;

Enemy::Enemy(EnemyType type, sf::Vector2f startPos, int splitGeneration)
    : type(type)
    , position(startPos)
    , splitGeneration(splitGeneration)
    , attackTimer(0.f)
    , attacking(false)
    , alive(true)
{
    initStats();
    shape.setOrigin(shape.getRadius(), shape.getRadius());
    shape.setPosition(position);
}

/* Edit Enemy Stats ------------------------ */
void Enemy::initStats() {
    switch (type) {

        case EnemyType::DustMummy:
            hp          = 60.f;
            speed       = 60.f;
            damage      = 15.f;
            attackSpeed = 0.8f;
            reward      = 10;
            shape.setRadius(12.f);
            animSprite.frames = { "dust_mummy_0", "dust_mummy_1", "dust_mummy_2", "dust_mummy_3", "dust_mummy_4", "dust_mummy_5" };
            animSprite.fps    = 6.f;
            {
                sf::Texture& t = SpriteManager::get().getTexture("dust_mummy_0");
                sf::Vector2u sz = t.getSize();
                animSprite.sprite.setTexture(t, true);
                animSprite.sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
                animSprite.sprite.setScale(1.5f, 1.5f);
            }
            break;

        case EnemyType::SporePuff: {
            static const float hpArr[]     = { 50.f, 25.f, 12.f };
            static const float dmgArr[]    = {  8.f,  4.f,  2.f };
            static const float radArr[]    = {  9.f,  7.f,  6.f };
            static const int   rewardArr[] = {    0,    0,   20  };
            hp          = hpArr[splitGeneration];
            damage      = dmgArr[splitGeneration];
            speed       = 60.f;
            attackSpeed = 1.2f;      
            reward      = rewardArr[splitGeneration];
            shape.setRadius(radArr[splitGeneration]);
            shape.setFillColor(sf::Color(180, 130, 200));
            shape.setOutlineColor(sf::Color(120, 60, 160));
            shape.setOutlineThickness(2.f);
            break;
        }

        case EnemyType::ShadowCrow:
            hp          = 25.f;         
            speed       = 220.f;        
            damage      = 5.f;
            attackSpeed = 2.0f;         
            reward      = 20;          
            shape.setRadius(6.f);
            shape.setFillColor(sf::Color(50, 50, 80));
            shape.setOutlineColor(sf::Color(100, 100, 180));
            shape.setOutlineThickness(1.5f);
            break;

        case EnemyType::RustGolem:
            hp          = 350.f;       
            speed       = 35.f;         
            damage      = 40.f;        
            attackSpeed = 0.3f;        
            reward      = 50;          
            shape.setRadius(15.f);
            shape.setFillColor(sf::Color(130, 100, 80));
            shape.setOutlineColor(sf::Color(80, 60, 40));
            shape.setOutlineThickness(3.f);
            break;
    }

    maxHp = hp;
}


void Enemy::update(float dt, sf::Vector2f target) {
    if (!alive) return;

    animSprite.update(dt);

    if (slowTimer > 0.f) slowTimer -= dt;
    else                 slowFactor = 1.f;

    sf::Vector2f dir = target - position;
    float        d   = dist(target, position);

    if (d <= ATTACK_RANGE) {
        attacking = true;
        if (attackTimer > 0.f) attackTimer -= dt;
    } else {
        attacking   = false;
        attackTimer = 0.f;
        dir        /= d;
        position   += dir * speed * slowFactor * dt;
        shape.setPosition(position);
    }
}


void Enemy::draw(sf::RenderWindow& window) {
    if (!alive) return;
    if (animSprite.hasFrames())
        animSprite.draw(window, position);
    else
        window.draw(shape);
    drawHpBar(window);
}


void Enemy::drawHpBar(sf::RenderWindow& window) {
    float r = shape.getRadius();
    drawHealthBar(window, position.x - r, position.y - r - 6.f,
                  r * 2.f, 4.f, hp / maxHp, sf::Color(60, 200, 80));
}


void Enemy::takeDamage(float amount) {
    hp -= amount;
    if (hp <= 0.f) { hp = 0.f; alive = false; }
}

void Enemy::applySlow(float factor, float duration) {
    slowFactor = factor;
    slowTimer  = duration;
}

bool Enemy::consumeAttack() {
    if (!attacking || attackTimer > 0.f) return false;
    attackTimer = 1.f / attackSpeed;   // set cooldown for next swing
    return true;
}

bool         Enemy::isAlive()            const { return alive; }
bool         Enemy::isAttacking()        const { return attacking; }
bool         Enemy::shouldSplit()        const { return type == EnemyType::SporePuff && splitGeneration < 2; }
sf::Vector2f Enemy::getPosition()        const { return position; }
float        Enemy::getDamage()          const { return damage; }
int          Enemy::getReward()          const { return reward; }
int          Enemy::getSplitGeneration() const { return splitGeneration; }
EnemyType    Enemy::getType()            const { return type; }