#pragma once
#include <SFML/Graphics.hpp>

class Tower {
public:
    Tower(sf::Vector2f worldPos);

    void draw(sf::RenderWindow& window);

    sf::Vector2f getPosition() const;

private:
    sf::Vector2f    position;
    sf::CircleShape shape;
};