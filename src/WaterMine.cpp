#include "WaterMine.h"

WaterMine::WaterMine(sf::Vector2f worldPos, sf::Font& font)
    : position(worldPos)
    , font(font)
{
    shape.setRadius(14.f);
    shape.setOrigin(14.f, 14.f);
    shape.setFillColor(sf::Color(0, 160, 160));
    shape.setOutlineColor(sf::Color(0, 220, 220));
    shape.setOutlineThickness(2.f);
    shape.setPosition(position);

    popCircle.setRadius(10.f);
    popCircle.setOrigin(10.f, 10.f);
    popCircle.setFillColor(sf::Color::White);
    popCircle.setOutlineColor(sf::Color(0, 160, 160));
    popCircle.setOutlineThickness(2.f);
    popCircle.setPosition(position.x, position.y - 30.f);

    popText.setFont(font);
    popText.setString("!");
    popText.setCharacterSize(14);
    popText.setFillColor(sf::Color(0, 100, 100));
    sf::FloatRect tb = popText.getLocalBounds();
    popText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    popText.setPosition(position.x, position.y - 30.f);
}

void WaterMine::update(float dt) {
    if (ready) return;
    harvestTimer += dt;
    if (harvestTimer >= harvestInterval) {
        harvestTimer = harvestInterval;
        ready = true;
    }
}

void WaterMine::draw(sf::RenderWindow& window) {
    window.draw(shape);
    if (ready) {
        window.draw(popCircle);
        window.draw(popText);
    }
}

bool         WaterMine::isReady()                       const { return ready; }
sf::Vector2f WaterMine::getPosition()                   const { return position; }
bool         WaterMine::contains(sf::Vector2f mousePos) const { return shape.getGlobalBounds().contains(mousePos); }

int WaterMine::collect() {
    ready        = false;
    harvestTimer = 0.f;
    return harvestAmount;
}
