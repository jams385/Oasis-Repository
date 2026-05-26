#pragma once
#include <SFML/Graphics.hpp>

enum class CornucopiaState { Broken, Active };

class Cornucopia {
public:
    static constexpr int NUM_CORNUCOPIAS = 4;
    static constexpr int RESTORE_COST    = 500;

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

    bool         isBroken()  const;
    bool         isActive()  const;
    sf::Vector2f getPosition() const;
    float        getHp()       const;
    float        getMaxHp()    const;

private:
    sf::Vector2f       position;
    float              hp;
    float              maxHp;
    CornucopiaState    cornState;
    bool               popupOpen;

    sf::RectangleShape body;
    sf::ConvexShape    top;
    sf::RectangleShape popupBox;

    void drawHpBar(sf::RenderWindow& window);
    void applyVisual();
};
