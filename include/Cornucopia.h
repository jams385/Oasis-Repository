#pragma once
#include <SFML/Graphics.hpp>

class Cornucopia {
public:
    Cornucopia(sf::Vector2f worldPos);

    void update(float dt);
    void draw(sf::RenderWindow& window);
    void takeDamage(float amount);

    bool         isDestroyed() const;
    sf::Vector2f getPosition() const;
    float        getHp()       const;
    float        getMaxHp()    const;

private:
    sf::Vector2f    position;
    float           hp;
    float           maxHp;

    sf::RectangleShape body;
    sf::ConvexShape    top;     // triangle crown

    void drawHpBar(sf::RenderWindow& window);
};
