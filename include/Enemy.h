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

    bool         isAlive()                          const;
    bool         hasReachedTarget(sf::Vector2f target) const;
    sf::Vector2f getPosition()                      const;
    float        getDamage()                        const;
    int          getReward()                        const;
    EnemyType    getType()                          const;

private:
    EnemyType       type;
    sf::Vector2f    position;
    float           speed;
    float           hp;
    float           maxHp;
    float           damage;
    int             reward;
    bool            alive;

    sf::CircleShape shape;

    void initStats();       
    void drawHpBar(sf::RenderWindow& window);
};