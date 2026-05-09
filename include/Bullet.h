#pragma once
#include <SFML/Graphics.hpp>

class Bullet {
public:
    Bullet(sf::Vector2f pos, sf::Vector2f velocity, float damage);

    void update(float dt);
    void draw(sf::RenderWindow& window);

    void         expire();
    bool         isExpired()   const;
    sf::Vector2f getPosition() const;
    float        getDamage()   const;

private:
    sf::Vector2f    position;
    sf::Vector2f    velocity;
    float           damage;
    bool            expired  = false;
    float           traveled = 0.f;
    static constexpr float MAX_RANGE = 300.f;

    sf::CircleShape shape;
};
