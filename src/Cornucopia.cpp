#include "Cornucopia.h"
#include "AudioManager.h"
#include <cmath>

static const float BODY_W  = 20.f;
static const float BODY_H  = 27.f;
static const float MAX_HP  = 300.f;
static const float POPUP_W = 150.f;
static const float POPUP_H = 44.f;

Cornucopia::Cornucopia(sf::Vector2f worldPos)
    : position(worldPos)
    , hp(MAX_HP)
    , maxHp(MAX_HP)
    , cornState(CornucopiaState::Broken)
    , popupOpen(false)
{
    body.setSize({ BODY_W, BODY_H });
    body.setOrigin(BODY_W / 2.f, BODY_H / 2.f);
    body.setPosition(position);

    top.setPointCount(3);
    top.setPoint(0, { 0.f,            -10.f });
    top.setPoint(1, { -BODY_W / 2.f,   0.f  });
    top.setPoint(2, {  BODY_W / 2.f,   0.f  });
    top.setOutlineThickness(1.5f);
    top.setPosition(position.x, position.y - BODY_H / 2.f);

    float popupX = position.x - POPUP_W / 2.f;
    float popupY = position.y - BODY_H / 2.f - 10.f - 6.f - POPUP_H;
    popupBox.setSize({ POPUP_W, POPUP_H });
    popupBox.setPosition(popupX, popupY);
    popupBox.setFillColor(sf::Color(20, 20, 20, 220));
    popupBox.setOutlineColor(sf::Color(255, 215, 0));
    popupBox.setOutlineThickness(2.f);

    applyVisual();
}

void Cornucopia::applyVisual() {
    if (cornState == CornucopiaState::Broken) {
        body.setFillColor(sf::Color(70, 65, 50));
        body.setOutlineColor(sf::Color(90, 90, 90));
        body.setOutlineThickness(2.f);
        top.setFillColor(sf::Color(100, 95, 70));
        top.setOutlineColor(sf::Color(90, 90, 90));
    } else {
        body.setFillColor(sf::Color(180, 130, 20));
        body.setOutlineColor(sf::Color(255, 215, 0));
        body.setOutlineThickness(2.5f);
        top.setFillColor(sf::Color(255, 215, 0));
        top.setOutlineColor(sf::Color(200, 160, 0));
    }
}

void Cornucopia::restore() {
    if (cornState != CornucopiaState::Broken) return;
    cornState = CornucopiaState::Active;
    hp        = maxHp;
    popupOpen = false;
    soldier.activate(position);
    applyVisual();
}

void Cornucopia::openPopup()  { popupOpen = true;  }
void Cornucopia::closePopup() { popupOpen = false; }
bool Cornucopia::isPopupOpen() const { return popupOpen; }

sf::FloatRect Cornucopia::getPopupBounds() const {
    return popupBox.getGlobalBounds();
}

bool Cornucopia::containsPoint(sf::Vector2f p) const {
    return body.getGlobalBounds().contains(p) || top.getGlobalBounds().contains(p);
}

static constexpr float HEAL_DELAY = 10.f;
static constexpr float HEAL_RATE  = 5.f;

void Cornucopia::update(float dt) {
    if (!isActive()) return;

    timeSinceDamage += dt;
    if (timeSinceDamage >= HEAL_DELAY && hp < maxHp) {
        hp += HEAL_RATE * dt;
        if (hp > maxHp) hp = maxHp;
    }
}

void Cornucopia::draw(sf::RenderWindow& window) {
    window.draw(top);
    window.draw(body);
    if (cornState == CornucopiaState::Active)
        drawHpBar(window);
    if (popupOpen)
        window.draw(popupBox);
}

void Cornucopia::drawHpBar(sf::RenderWindow& window) {
    float ratio = hp / maxHp;
    float barW  = BODY_W + 9.f;
    float barH  = 5.f;
    float x     = position.x - barW / 2.f;
    float y     = position.y - BODY_H / 2.f - 16.f;

    sf::RectangleShape bg({ barW, barH });
    bg.setFillColor(sf::Color(80, 0, 0));
    bg.setPosition(x, y);
    window.draw(bg);

    sf::RectangleShape bar({ barW * ratio, barH });
    bar.setFillColor(sf::Color(255, 165, 0));
    bar.setPosition(x, y);
    window.draw(bar);
}

void Cornucopia::takeDamage(float amount) {
    if (cornState != CornucopiaState::Active) return;
    timeSinceDamage = 0.f;
    hp -= amount;
    if (hp <= 0.f) {
        hp        = maxHp; // reset so it starts fresh when restored
        cornState = CornucopiaState::Broken;
        soldier.deactivate();
        applyVisual();
    }
}

bool         Cornucopia::isBroken()    const { return cornState == CornucopiaState::Broken; }
bool         Cornucopia::isActive()    const { return cornState == CornucopiaState::Active; }
sf::Vector2f Cornucopia::getPosition() const { return position; }
float        Cornucopia::getHp()       const { return hp; }
float        Cornucopia::getMaxHp()    const { return maxHp; }

void Cornucopia::updateSoldier(float dt, std::vector<Enemy>& enemies) { soldier.update(dt, enemies); }
void Cornucopia::drawSoldier(sf::RenderWindow& window)                { soldier.draw(window); }
void Cornucopia::drawSoldierRadius(sf::RenderWindow& window)          { soldier.drawPatrolRadius(window); }
void Cornucopia::orderSoldierTo(sf::Vector2f p)                       { soldier.orderMoveTo(p); }
bool Cornucopia::soldierContainsPoint(sf::Vector2f p) const           { return soldier.containsPoint(p); }
bool Cornucopia::hasSoldier()                         const           { return soldier.isActive(); }
