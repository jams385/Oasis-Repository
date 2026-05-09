#pragma once
#include <SFML/Graphics.hpp>

enum class EnemyType {
    DustMummy,
    SporePuff,
    ShadowCrow,
    RustGolem
};

class Enemy {
public:
    Enemy(EnemyType type, sf::Vector2f startPos, int splitGeneration = 0);

    void update(float dt, sf::Vector2f target);
    void draw(sf::RenderWindow& window);

    void takeDamage(float amount);
    
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
    float           damage;       // damage per melee hit
    float           attackSpeed;  // hits per second
    float           attackTimer;  // counts down; fires when <= 0
    bool            attacking;    // true while in melee range
    int             reward;
    bool            alive;

    sf::CircleShape shape;

    void initStats();
    void drawHpBar(sf::RenderWindow& window);
};