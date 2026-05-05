#pragma once
#include <SFML/Graphics.hpp>

class Cornucopia {
public:
    Cornucopia(sf::Vector2f pos, float hp);

    void takeDamage(float amount);
    void draw(sf::RenderWindow& window);

    bool isAlive() const;
    float getHp() const;
    float getMaxHp() const;
    sf::Vector2f getPosition() const;

private:
    sf::Vector2f position;
    float        hp;
    float        maxHp;

    sf::RectangleShape shape;
};