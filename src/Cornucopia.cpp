#include "Cornucopia.h"

Cornucopia::Cornucopia(sf::Vector2f pos, float hp)
    : position(pos), hp(hp), maxHp(hp)
{
    // Placeholder: gold rectangle
    shape.setSize({ 40.f, 60.f });
    shape.setOrigin(20.f, 30.f);
    shape.setFillColor(sf::Color(200, 160, 30));
    shape.setOutlineColor(sf::Color(255, 215, 0));
    shape.setOutlineThickness(3.f);
    shape.setPosition(position);
}

void Cornucopia::takeDamage(float amount) {
    hp -= amount;
    if (hp < 0.f) hp = 0.f;
}

void Cornucopia::draw(sf::RenderWindow& window) {
    window.draw(shape);

    // HP bar above the structure
    float ratio = hp / maxHp;

    sf::RectangleShape hpBg({ 60.f, 8.f });
    hpBg.setFillColor(sf::Color(80, 0, 0));
    hpBg.setPosition(position.x - 30.f, position.y - 50.f);
    window.draw(hpBg);

    sf::RectangleShape hpBar({ 60.f * ratio, 8.f });
    hpBar.setFillColor(sf::Color(60, 200, 80));
    hpBar.setPosition(position.x - 30.f, position.y - 50.f);
    window.draw(hpBar);
}

bool Cornucopia::isAlive() const {
    return hp > 0.f;
}

float Cornucopia::getHp()    const { 
    return hp; 
}

float Cornucopia::getMaxHp() const { 
    return maxHp; 
}

sf::Vector2f Cornucopia::getPosition() const { 
    return position; 
}