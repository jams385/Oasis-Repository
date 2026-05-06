#include "Tower.h"

Tower::Tower(sf::Vector2f worldPos)
    : position(worldPos)
{
    shape.setRadius(14.f);
    shape.setOrigin(14.f, 14.f);
    shape.setFillColor(sf::Color(30, 100, 200));
    shape.setOutlineColor(sf::Color(100, 180, 255));
    shape.setOutlineThickness(2.f);
    shape.setPosition(position);
}

void Tower::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::Vector2f Tower::getPosition() const { return position; }