#include "WaterMine.h"
#include "AudioManager.h"
#include "GameUtils.h"
#include <cstdlib>
#include <string>

WaterMine::WaterMine(sf::Vector2f worldPos, sf::Font& font)
    : position(worldPos)
    , font(&font)
{
    shape.setRadius(10.f);
    shape.setOrigin(10.f, 10.f);
    shape.setFillColor(sf::Color(0, 160, 160));
    shape.setOutlineColor(sf::Color(0, 220, 220));
    shape.setOutlineThickness(2.f);
    shape.setPosition(position);

    popCircle.setRadius(8.f);
    popCircle.setOrigin(8.f, 8.f);
    popCircle.setFillColor(sf::Color::White);
    popCircle.setOutlineColor(sf::Color(0, 160, 160));
    popCircle.setOutlineThickness(2.f);
    popCircle.setPosition(position.x, position.y - 22.f);

    popText.setFont(font);
    popText.setString("!");
    popText.setCharacterSize(14);
    popText.setFillColor(sf::Color(0, 100, 100));
    sf::FloatRect tb = popText.getLocalBounds();
    popText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    popText.setPosition(position.x, position.y - 22.f);

    collectText.setFont(font);
    collectText.setCharacterSize(14);
    collectText.setStyle(sf::Text::Bold);
}

void WaterMine::update(float dt) {
    if (isDestroyed()) return;

    timeSinceDamage += dt;
    if (timeSinceDamage >= HEAL_DELAY && hp < maxHp) {
        hp += HEAL_RATE * dt;
        if (hp > maxHp) hp = maxHp;
    }

    if (popupTimer > 0.f) {
        popupTimer -= dt;
        popupY -= 30.f * dt;
        collectText.setPosition(position.x, popupY);
        float alpha = std::min(1.f, popupTimer / 0.4f) * 255.f;
        sf::Color c = collectText.getFillColor();
        c.a = static_cast<sf::Uint8>(alpha);
        collectText.setFillColor(c);
        if (popupTimer < 0.f) popupTimer = 0.f;
    }

    if (ready) return;
    harvestTimer += dt;
    if (harvestTimer >= harvestInterval) {
        harvestTimer = harvestInterval;
        ready = true;
    }
}

void WaterMine::draw(sf::RenderWindow& window) {
    if (isDestroyed()) return;
    window.draw(shape);
    drawHpBar(window);
    if (ready) {
        window.draw(popCircle);
        window.draw(popText);
    }
    if (popupTimer > 0.f) {
        window.draw(collectText);
    }
}

void WaterMine::takeDamage(float amount) {
    hp -= amount;
    if (hp <= 0.f) hp = 0.f;
    timeSinceDamage = 0.f;
}

bool WaterMine::isDestroyed() const { return hp <= 0.f; }

void WaterMine::drawHpBar(sf::RenderWindow& window) {
    float r = shape.getRadius();
    drawHealthBar(window, position.x - r, position.y - r - 6.f,
                  r * 2.f, 4.f, hp / maxHp, sf::Color(255, 100, 50));
}

bool         WaterMine::isReady()                       const { return ready; }
sf::Vector2f WaterMine::getPosition()                   const { return position; }
bool         WaterMine::contains(sf::Vector2f mousePos) const { return shape.getGlobalBounds().contains(mousePos); }

int WaterMine::collect() {
    ready        = false;
    harvestTimer = 0.f;

    int amount = 0;

    if(rand() % 10 < 2){

        AudioManager::get().play("WaterMine_Collect_25");
        amount = 25;

    } else { amount = 10; AudioManager::get().play("WaterMine_Collect");}

    popupAmount = amount;
    popupTimer  = 1.2f;
    popupY      = position.y - 22.f;

    collectText.setString("+" + std::to_string(amount));
    sf::Color col = (amount == 25) ? sf::Color(255, 215, 0) : sf::Color(0, 220, 220);
    col.a = 255;
    collectText.setFillColor(col);
    sf::FloatRect tb = collectText.getLocalBounds();
    collectText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    collectText.setPosition(position.x, popupY);

    return amount;
}
