#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Enemy.h"

enum class SoldierState { Inactive, Alive, Dead };

class Soldier {
public:
    static constexpr float PATROL_RADIUS  = 100.f;
    static constexpr float RESPAWN_TIME   = 15.f;

    Soldier();

    void activate(sf::Vector2f home);
    void deactivate();

    void update(float dt, std::vector<Enemy>& enemies);
    void draw(sf::RenderWindow& window);
    void drawPatrolRadius(sf::RenderWindow& window);

    void         orderMoveTo(sf::Vector2f worldPos);
    bool         isActive()                    const;
    bool         containsPoint(sf::Vector2f p) const;
    sf::Vector2f getPosition()                 const;
    sf::Vector2f getHome()                     const;

private:
    sf::Vector2f    home;
    sf::Vector2f    position;
    sf::Vector2f    targetPos;
    SoldierState    state          = SoldierState::Inactive;

    float           hp             = 0.f;
    float           maxHp          = 50.f;
    float           moveSpeed      = 80.f;
    float           attackRange    = 35.f;
    float           damage         = 12.f;
    float           attackTimer    = 0.f;
    float           attackCooldown = 1.0f;
    float           respawnTimer   = 0.f;

    sf::CircleShape shape;
    sf::CircleShape patrolCircle;

    void drawHpBar(sf::RenderWindow& window);
};
