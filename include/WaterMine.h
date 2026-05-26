#pragma once
#include <SFML/Graphics.hpp>

class WaterMine {
public:
    WaterMine(sf::Vector2f worldPos, sf::Font& font);

    void         update(float dt);
    void         draw(sf::RenderWindow& window);
    bool         isReady()                       const;
    int          collect();
    bool         contains(sf::Vector2f mousePos) const;
    sf::Vector2f getPosition()                   const;

private:
    sf::Vector2f    position;
    sf::Font&       font;

    float           harvestTimer    = 0.f;
    float           harvestInterval = 15.f;
    int             harvestAmount   = 25;
    bool            ready           = false;

    sf::CircleShape shape;
    sf::CircleShape popCircle;
    sf::Text        popText;
};
