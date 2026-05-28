#pragma once
#include <SFML/Graphics.hpp>
#include "Soldier.h"

enum class CornucopiaState { Broken, Active };

class Cornucopia {
public:
    static constexpr int NUM_CORNUCOPIAS  = 5;
    static constexpr int RESTORE_COST_BASE = 0;

    static int getRestoreCost(int activeCount) {
        return RESTORE_COST_BASE * activeCount;
    }

    Cornucopia(sf::Vector2f worldPos);

    void update(float dt);
    void draw(sf::RenderWindow& window);
    void takeDamage(float amount);
    void restore();

    void openPopup();
    void closePopup();
    bool isPopupOpen()           const;
    sf::FloatRect getPopupBounds() const;

    bool containsPoint(sf::Vector2f p) const;

    void updateSoldier(float dt, std::vector<Enemy>& enemies);
    void drawSoldier(sf::RenderWindow& window);
    void drawSoldierRadius(sf::RenderWindow& window);
    void orderSoldierTo(sf::Vector2f worldPos);
    bool         isBroken()    const;
    bool         isActive()    const;
    sf::Vector2f getPosition() const;

private:
    sf::Vector2f       position;
    float              hp;
    float              maxHp;
    float              timeSinceDamage = 0.f;
    CornucopiaState    cornState;
    bool               popupOpen;

    sf::RectangleShape body;
    sf::ConvexShape    top;
    sf::RectangleShape popupBox;
    Soldier            soldier;

    void drawHpBar(sf::RenderWindow& window);
    void applyVisual();
};
