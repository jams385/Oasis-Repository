#include "Cornucopia.h"
#include <cmath>

static const float BODY_W  = 26.f;
static const float BODY_H  = 36.f;
static const float MAX_HP  = 300.f;

Cornucopia::Cornucopia(sf::Vector2f worldPos)
    : position(worldPos)
    , hp(MAX_HP)
    , maxHp(MAX_HP)
{
    body.setSize({ BODY_W, BODY_H });
    body.setOrigin(BODY_W / 2.f, BODY_H / 2.f);
    body.setFillColor(sf::Color(180, 130, 20));
    body.setOutlineColor(sf::Color(255, 215, 0));
    body.setOutlineThickness(2.5f);
    body.setPosition(position);

    // Triangle crown sitting on top of the body
    top.setPointCount(3);
    top.setPoint(0, { 0.f,            -14.f }); 
    top.setPoint(1, { -BODY_W / 2.f,   0.f  });   
    top.setPoint(2, {  BODY_W / 2.f,   0.f  });  
    top.setFillColor(sf::Color(255, 215, 0));
    top.setOutlineColor(sf::Color(200, 160, 0));
    top.setOutlineThickness(1.5f);
   
    top.setPosition(position.x, position.y - BODY_H / 2.f);
}

/* TO BE USED LATER ON */
void Cornucopia::update(float /*dt*/) {}

void Cornucopia::draw(sf::RenderWindow& window) {
    if (isDestroyed()) return;
    window.draw(top);
    window.draw(body);
    drawHpBar(window);
}

void Cornucopia::drawHpBar(sf::RenderWindow& window) {
    float ratio    = hp / maxHp;
    float barW     = BODY_W + 12.f;
    float barH     = 5.f;
    float x        = position.x - barW / 2.f;
    float y        = position.y - BODY_H / 2.f - 22.f;

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
    hp -= amount;
    if (hp < 0.f) hp = 0.f;
}

bool         Cornucopia::isDestroyed() const { return hp <= 0.f; }
sf::Vector2f Cornucopia::getPosition() const { return position; }
float        Cornucopia::getHp()       const { return hp; }
float        Cornucopia::getMaxHp()    const { return maxHp; }
