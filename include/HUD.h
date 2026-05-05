#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// Which tower the player currently has selected
enum class SelectedTower {
    WaterTower,
    SunBeam,
    TreeTower
};

class HUD {
public:
    HUD(sf::Font& font, int windowWidth, int windowHeight);

    void update(float dt, int waterPoints, int waveNumber, bool newWave);
    void draw(sf::RenderWindow& window);

    // Returns which tower button was clicked, or -1 if none
    SelectedTower getSelectedTower() const;
    bool handleClick(sf::Vector2f mousePos);  // returns true if a button was clicked

    // Day/night cycle
    void tickDayNight(float dt);
    bool isNight() const;

private:
    sf::Font&    font;
    int          windowWidth;
    int          windowHeight;

    // ── Currency ──────────────────────────────────────────────────────────────
    int          waterPoints = 0;
    sf::Text     currencyText;

    // ── Wave announcement ─────────────────────────────────────────────────────
    int          waveNumber  = 1;
    float        announcementTimer = 0.f;  // counts down after a new wave starts
    sf::Text     waveText;
    sf::Text     waveAnnouncement;

    // ── Day/Night bar ─────────────────────────────────────────────────────────
    float        dayNightProgress = 0.f;   // 0.0 = start of day, 1.0 = end of night
    float        cycleDuration    = 30.f;  // full day+night cycle in seconds
    bool         _isNight         = false;
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
};