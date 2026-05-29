#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Tower.h"

class HUD {
public:
    HUD(sf::Font& font, int windowWidth, int windowHeight);

    void update(float dt, int waterPoints, int waveNumber, bool newWave, int restoredCount, int totalCornucopias, int mineCount, int mineCap);
    void draw(sf::RenderWindow& window);

    TowerType getSelectedTower() const;
    bool      handleClick(sf::Vector2f mousePos);
    void      deselect();
    bool      hasSelection() const;
    void      reset();

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
    sf::Texture currencyTexture;
    sf::Sprite  currencySprite;

    // ── Cornucopia count ──────────────────────────────────────────────────────
    int          restoredCount    = 0;
    int          totalCornucopias = 0;
    sf::Text     cornucopiaText;

    // ── Mine cap ──────────────────────────────────────────────────────────────
    int          mineCount = 0;
    int          mineCap   = 5;
    sf::Text     mineCountText;

    // ── Wave ──────────────────────────────────────────────────────────────────
    int          waveNumber  = 1;
    float        announcementTimer = 0.f;
    sf::Text     waveText;
    sf::Text     waveAnnouncement;

    // ── Day/Night bar ─────────────────────────────────────────────────────────
    float        dayNightProgress = 0.f;
    float        dayDuration      = 20.f;
    float        nightDuration    = 50.f;
    bool         _isNight         = false;
    bool         _cycleCompleted  = false;
    sf::RectangleShape dayNightBg;
    sf::RectangleShape dayNightBar;
    sf::Text           dayNightLabel;

///SAMPLEEEE
sf::Texture waterTexture;
sf::Texture sunTexture;
sf::Texture treeTexture;
sf::Texture mineTexture;
//////////

    // ── Structure buttons ─────────────────────────────────────────────────────
    struct TowerButton {
        TowerType          type;
        sf::Text nameText;
        sf::Text costText;
        sf::RectangleShape shape;
        //sf::Texture iconTexture;
        sf::Sprite  iconSprite;
        
        bool selected = false;
    };
    std::vector<TowerButton> buttons;
    TowerType                selectedTower = TowerType::WaterTower;
    bool                     _hasSelection = false;

    void buildButtons();
    void drawDayNightBar(sf::RenderWindow& window);
    void drawCurrency(sf::RenderWindow& window);
    void drawWaveInfo(sf::RenderWindow& window);
    void drawStructureButtons(sf::RenderWindow& window);
    void drawCornucopiaCount(sf::RenderWindow& window);
    
};