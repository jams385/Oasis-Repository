#include "Cornucopia.h"
#include "AudioManager.h"
#include "GameUtils.h"
#include "SpriteManager.h"

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

    {
        sf::Texture& t = SpriteManager::get().getTexture("cornucopia_main");
        cornucopiaMainSprite.setTexture(t, true);
        cornucopiaMainSprite.setOrigin(t.getSize().x / 2.f, t.getSize().y / 2.f);
        cornucopiaMainSprite.setPosition(position);
        cornucopiaMainSprite.setScale(1.5f, 1.5f);
    }
    {
        sf::Texture& t = SpriteManager::get().getTexture("cornucopia_mini");
        cornucopiaMiniSprite.setTexture(t, true);
        cornucopiaMiniSprite.setOrigin(t.getSize().x / 2.f, t.getSize().y / 2.f);
        cornucopiaMiniSprite.setPosition(position);
        cornucopiaMiniSprite.setScale(0.5f, 0.5f);
    }


    float popupX = position.x - POPUP_W / 2.f;
    float popupY = position.y - BODY_H / 2.f - 10.f - 6.f - POPUP_H;
    popupBox.setSize({ POPUP_W, POPUP_H });
    popupBox.setPosition(popupX, popupY);
    popupBox.setFillColor(sf::Color(20, 20, 20, 220));
    popupBox.setOutlineColor(sf::Color(255, 215, 0));
    popupBox.setOutlineThickness(2.f);

    applyVisual();
}

void Cornucopia::applyVisual()
{
    if (cornState == CornucopiaState::Broken)
        cornucopiaMiniSprite.setColor(sf::Color(150, 150, 150)); // grey tint
    else
        cornucopiaMainSprite.setColor(sf::Color::White);
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

bool Cornucopia::containsPoint(sf::Vector2f p) const
{
    if (cornState == CornucopiaState::Broken)
        return cornucopiaMiniSprite.getGlobalBounds().contains(p);

    return cornucopiaMainSprite.getGlobalBounds().contains(p);
}

void Cornucopia::update(float dt) {
    if (!isActive()) return;

    timeSinceDamage += dt;
    if (timeSinceDamage >= HEAL_DELAY && hp < maxHp) {
        hp += HEAL_RATE * dt;
        if (hp > maxHp) hp = maxHp;
    }
}

void Cornucopia::draw(sf::RenderWindow& window)
{
    if (cornState == CornucopiaState::Broken)
        window.draw(cornucopiaMiniSprite);
    else
        window.draw(cornucopiaMainSprite);
    if (cornState == CornucopiaState::Active)
        drawHpBar(window);
    if (popupOpen)
        window.draw(popupBox);
}


void Cornucopia::drawHpBar(sf::RenderWindow& window)
{
    sf::FloatRect bounds =
        cornucopiaMainSprite.getGlobalBounds();
    drawHealthBar(
        window,
        position.x - bounds.width / 2.f,
        position.y - bounds.height / 2.f - 10.f,
        bounds.width,
        5.f,
        hp / maxHp,
        sf::Color(255, 165, 0)
    );
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

void Cornucopia::updateSoldier(float dt, std::vector<Enemy>& enemies) { soldier.update(dt, enemies); }
void Cornucopia::drawSoldier(sf::RenderWindow& window)                { soldier.draw(window); }
void Cornucopia::drawSoldierRadius(sf::RenderWindow& window)          { soldier.drawPatrolRadius(window); }
void Cornucopia::orderSoldierTo(sf::Vector2f p)                       { soldier.orderMoveTo(p); }
