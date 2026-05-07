#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// Which structure the player currently has selected
enum class SelectedTower {
    WaterTower,
    SunBeam,
    TreeTower,
    Cornucopia
};

class HUD {
public:
    HUD(sf::Font& font, int windowWidth, int windowHeight);

    void update(float dt, int waterPoints, int waveNumber, bool newWave, int cornucopiaCount);
    void draw(sf::RenderWindow& window);

    SelectedTower getSelectedTower() const;
    int           getSelectedCost()  const;
    bool handleClick(sf::Vector2f mousePos);

    // Day/night cycle
    void tickDayNight(float dt);
    bool isNight() const;
    bool cycleJustCompleted();  // returns true once per full cycle

private:
    sf::Font&    font;
    int          windowWidth;
    int          windowHeight;

    // ── Currency ──────────────────────────────────────────────────────────────
    int          waterPoints = 0;
    sf::Text     currencyText;

    // ── Cornucopia count ──────────────────────────────────────────────────────
    int          cornucopiaCount = 0;
    sf::Text     cornucopiaText;

    // ── Wave ──────────────────────────────────────────────────────────────────
    int          waveNumber  = 1;
    float        announcementTimer = 0.f;
    sf::Text     waveText;
    sf::Text     waveAnnouncement;

    // ── Day/Night bar ─────────────────────────────────────────────────────────
    float        dayNightProgress = 0.f;
    float        cycleDuration    = 30.f;  // seconds for a full day+night cycle
    bool         _isNight         = false;
    bool         _cycleCompleted  = false; // true for one frame when cycle resets
    sf::RectangleShape dayNightBg;
    sf::RectangleShape dayNightBar;
    sf::Text           dayNightLabel;

    // ── Structure buttons ─────────────────────────────────────────────────────
    struct TowerButton {
        sf::RectangleShape shape;
        sf::Text           nameText;
        sf::Text           costText;
        SelectedTower      type;
        int                cost;
        bool               selected = false;
    };
    std::vector<TowerButton> buttons;
    SelectedTower            selectedTower = SelectedTower::WaterTower;

    void buildButtons();
    void drawDayNightBar(sf::RenderWindow& window);
    void drawCurrency(sf::RenderWindow& window);
    void drawWaveInfo(sf::RenderWindow& window);
    void drawStructureButtons(sf::RenderWindow& window);
    void drawCornucopiaCount(sf::RenderWindow& window);
};