#pragma once
#include <SFML/Graphics.hpp>
#include "AnimatedSprite.h"
#include <vector>

class Map;

enum class EnemyType {
    DustMummy,
    SporePuff,
    ShadowCrow,
    RustGolem
};

class Enemy {
public:
    Enemy(EnemyType type, sf::Vector2f startPos, int splitGeneration = 0);

    void update(float dt, sf::Vector2f target, const Map& map);
    void draw(sf::RenderWindow& window);

    void takeDamage(float amount);
    void applySlow(float factor, float duration);

    bool consumeAttack();

    bool         isAlive()           const;
    bool         isAttacking()       const;
    sf::Vector2f getPosition()       const;
    float        getDamage()         const;
    int          getReward()         const;
    EnemyType    getType()           const;
    bool         shouldSplit()       const;
    int          getSplitGeneration() const;

private:
    EnemyType       type;
    sf::Vector2f    position;
    int             splitGeneration;
    float           speed;
    float           hp;
    float           maxHp;
    float           damage;       
    float           attackSpeed;
    float           attackTimer;
    bool            attacking;
    int             reward;
    bool            alive;
    
    float           slowTimer    = 0.f;
    float           slowFactor   = 1.f;

    std::vector<sf::Vector2f> waypoints_;
    int                       waypointIdx_ = 0;
    float                     pathAge_     = 999.f;  // forces path on first update
    sf::Vector2f              lastTarget_;
    static constexpr float    PATH_RECOMPUTE = 1.5f;

    sf::CircleShape shape;
    AnimatedSprite  animSprite;

    void initStats();
    void drawHpBar(sf::RenderWindow& window);
};