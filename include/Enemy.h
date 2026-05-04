#pragma once
#include <SFML/Graphics.hpp>

class Enemy {
public:
    Enemy(sf::Vector2f startPos, float speed, float hp);

    void update(float dt);
    void draw(sf::RenderWindow& window);

    bool isAlive() const;
    sf::Vector2f getPosition() const;
    void takeDamage(float amount);

private:
    sf::Vector2f    position;
    float           speed;
    float           hp;
    bool            alive;
    sf::CircleShape shape;
};