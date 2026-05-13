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

    sf::CircleShape shape;

    void initStats();
    void drawHpBar(sf::RenderWindow& window);
};