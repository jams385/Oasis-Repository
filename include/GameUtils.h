#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

inline constexpr float HEAL_DELAY = 10.f;
inline constexpr float HEAL_RATE  = 5.f;

inline float dist(sf::Vector2f a, sf::Vector2f b) {
    sf::Vector2f d = a - b;
    return std::sqrt(d.x*d.x + d.y*d.y);
}

inline void drawHealthBar(sf::RenderWindow& window, float x, float y,
                          float barW, float barH, float ratio, sf::Color color)
{
    sf::RectangleShape bg({ barW, barH });
    bg.setFillColor(sf::Color(80, 0, 0));
    bg.setPosition(x, y);
    window.draw(bg);

    sf::RectangleShape bar({ barW * ratio, barH });
    bar.setFillColor(color);
    bar.setPosition(x, y);
    window.draw(bar);
}
