#include "Enemy.h"
#include "Map.h"
#include "Pathfinder.h"
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
            hp          = 40.f;
            speed       = 70.f;
            damage      = 10.f;
            attackSpeed = 0.8f;
            reward      = 12;
            shape.setRadius(12.f);
            animSprite.frames = { "dust_mummy_0", "dust_mummy_1", "dust_mummy_2", "dust_mummy_3", "dust_mummy_4", "dust_mummy_5" };
            animSprite.fps    = 6.f;
            {
                sf::Texture& t = SpriteManager::get().getTexture("dust_mummy_0");
                sf::Vector2u sz = t.getSize();
                animSprite.sprite.setTexture(t, true);
                animSprite.sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
                animSprite.sprite.setScale(1.f, 1.f);
            }
            break;

        case EnemyType::SporePuff: {
            static const float hpArr[]     = { 50.f, 25.f, 12.f };
            static const float dmgArr[]    = {  8.f,  4.f,  2.f };
            static const float radArr[]    = {  9.f,  7.f,  6.f };
            static const int   rewardArr[] = {    0,    0,   20  };
            hp          = hpArr[splitGeneration];
            damage      = dmgArr[splitGeneration];
            speed       = 55.f;
            attackSpeed = 1.2f;      
            reward      = rewardArr[splitGeneration];

            shape.setRadius(radArr[splitGeneration]);
            animSprite.frames = {"spore_puff_0","spore_puff_1","spore_puff_2","spore_puff_3","spore_puff_4","spore_puff_5"};
            animSprite.fps = 5.f;

            {
                sf::Texture& t = SpriteManager::get().getTexture("spore_puff_0");
                sf::Vector2u sz = t.getSize();
                animSprite.sprite.setTexture(t, true);
                animSprite.sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
                float scale = 1.f;
                if (splitGeneration == 1) scale = 0.7f;
                if (splitGeneration == 2) scale = 0.45f;
                animSprite.sprite.setScale(scale, scale);
            }
    break;
}

        case EnemyType::ShadowCrow:
            hp          = 25.f;         
            speed       = 220.f;        
            damage      = 6.f;
            attackSpeed = 2.0f;         
            reward      = 20;          
                shape.setRadius(6.f);

            animSprite.frames = {"shadow_crow_0","shadow_crow_1","shadow_crow_2","shadow_crow_3"};
            animSprite.fps = 8.f;
            {
                sf::Texture& t = SpriteManager::get().getTexture("shadow_crow_0");
                sf::Vector2u sz = t.getSize();
                animSprite.sprite.setTexture(t, true);
                animSprite.sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
                animSprite.sprite.setScale(1.5f, 1.5f);
            }
            break;

        case EnemyType::RustGolem:
            hp          = 302.f;       
            speed       = 35.f;         
            damage      = 40.f;        
            attackSpeed = 0.3f;        
            reward      = 50; 
            
            shape.setRadius(15.f);

            animSprite.frames = {"rust_golem_0","rust_golem_1","rust_golem_2","rust_golem_3","rust_golem_4","rust_golem_5"};
            animSprite.fps = 5.f;
            {
                sf::Texture& t = SpriteManager::get().getTexture("rust_golem_0");
                sf::Vector2u sz = t.getSize();
                animSprite.sprite.setTexture(t, true);
                animSprite.sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
                animSprite.sprite.setScale(1.5f, 1.5f);
            }
            break;

    }
    maxHp = hp;
}


void Enemy::update(float dt, sf::Vector2f target, const Map& map) {
    if (!alive) return;

    animSprite.update(dt);

    if (slowTimer > 0.f) slowTimer -= dt;
    else                 slowFactor = 1.f;

    // Recompute path if stale or target moved to a different tile
    pathAge_ += dt;
    if (pathAge_ >= PATH_RECOMPUTE || dist(target, lastTarget_) > TILE_SIZE * 2.f) {
        waypoints_   = Pathfinder::findPath(map, position, target);
        waypointIdx_ = 0;
        pathAge_     = 0.f;
        lastTarget_  = target;
    }

    // Advance along waypoints
    sf::Vector2f moveTarget = target;
    if (!waypoints_.empty() && waypointIdx_ < (int)waypoints_.size()) {
        moveTarget = waypoints_[waypointIdx_];
        if (dist(position, moveTarget) < TILE_SIZE * 0.6f)
            waypointIdx_++;
    }

    float distToTarget = dist(target, position);
    if (distToTarget <= ATTACK_RANGE) {
        attacking = true;
        if (attackTimer > 0.f) attackTimer -= dt;
    } else {
        attacking   = false;
        attackTimer = 0.f;
        sf::Vector2f dir = moveTarget - position;
        float        d   = dist(moveTarget, position);
        if (d > 1.f) {
            dir      /= d;
            position += dir * speed * slowFactor * dt;
            shape.setPosition(position);
        }
    }

    //for facing relative to origin
    float absX = std::abs(animSprite.sprite.getScale().x);
    float y    = animSprite.sprite.getScale().y;
    if (target.x < position.x)animSprite.sprite.setScale(absX, y);
    else animSprite.sprite.setScale(-absX, y);
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