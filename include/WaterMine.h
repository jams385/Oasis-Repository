#pragma once
#include <SFML/Graphics.hpp>
#include "SpriteManager.h"

class WaterMine {
public:
    WaterMine(sf::Vector2f worldPos, sf::Font& font);

    void         update(float dt);
    void         draw(sf::RenderWindow& window);
    bool         isReady()                       const;
    bool         isDestroyed()                   const;
    void         takeDamage(float amount);
    int          collect();
    bool         contains(sf::Vector2f mousePos) const;
    sf::Vector2f getPosition()                   const;

private:
    sf::Vector2f    position;
    sf::Font*       font;

    float           harvestTimer    = 0.f;
    float           harvestInterval = 15.f;
    bool            ready           = false;

    float           hp              = 100.f;
    float           maxHp           = 100.f;
    float           timeSinceDamage = 0.f;

    float           popupTimer      = 0.f;
    float           popupY          = 0.f;
    int             popupAmount     = 0;

    sf::Sprite      sprite;
    sf::CircleShape popCircle;
    sf::Text        popText;
    sf::Text        collectText;

    void drawHpBar(sf::RenderWindow& window);
};
