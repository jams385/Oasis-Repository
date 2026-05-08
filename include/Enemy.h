#pragma once
#include <SFML/Graphics.hpp>

enum class EnemyType {
    DustMummy,    
    SporePuff,    
    ShadowCrow    
};

class Enemy {
public:
    Enemy(EnemyType type, sf::Vector2f startPos);

    void update(float dt, sf::Vector2f target);
    void draw(sf::RenderWindow& window);

    void takeDamage(float amount);

    // Returns true once per attack interval when the enemy is in melee range.
    // Call each frame; resets the cooldown internally.
    bool consumeAttack();

    bool         isAlive()     const;
    bool         isAttacking() const;
    sf::Vector2f getPosition() const;
    float        getDamage()   const;
    int          getReward()   const;
    EnemyType    getType()     const;

private:
    EnemyType       type;
    sf::Vector2f    position;
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