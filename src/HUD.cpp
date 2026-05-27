#include "HUD.h"
#include <sstream>

// ── Button layout constants ───────────────────────────────────────────────────
const float BTN_WIDTH   = 110.f;
const float BTN_HEIGHT  = 70.f;
const float BTN_PADDING = 12.f;
const float BTN_Y_OFFSET = 20.f;

// ── Constructor ───────────────────────────────────────────────────────────────
HUD::HUD(sf::Font& font, int windowWidth, int windowHeight)
    : font(font), windowWidth(windowWidth), windowHeight(windowHeight)
{
    // Currency text - top right
    currencyText.setFont(font);
    currencyText.setCharacterSize(22);
    currencyText.setFillColor(sf::Color::Cyan);

    // Small wave label - top middle
    waveText.setFont(font);
    waveText.setCharacterSize(20);
    waveText.setFillColor(sf::Color::White);

    // Big wave announcement - flashes on new wave
    waveAnnouncement.setFont(font);
    waveAnnouncement.setCharacterSize(52);
    waveAnnouncement.setFillColor(sf::Color::Yellow);

    // Day/night bar background
    dayNightBg.setSize({ 200.f, 18.f });
    dayNightBg.setFillColor(sf::Color(30, 30, 50));
    dayNightBg.setOutlineColor(sf::Color(100, 100, 150));
    dayNightBg.setOutlineThickness(1.5f);
    dayNightBg.setPosition(20.f, windowHeight - 50.f);

    // Day/night bar fill (starts empty)
    dayNightBar.setSize({ 0.f, 18.f });
    dayNightBar.setPosition(20.f, windowHeight - 50.f);

    // Day/night label above the bar
    dayNightLabel.setFont(font);
    dayNightLabel.setCharacterSize(16);
    dayNightLabel.setFillColor(sf::Color::White);
    dayNightLabel.setPosition(20.f, windowHeight - 74.f);

    // Cornucopia count - top left
    cornucopiaText.setFont(font);
    cornucopiaText.setCharacterSize(20);
    cornucopiaText.setFillColor(sf::Color(255, 215, 0));
    cornucopiaText.setPosition(20.f, 14.f);

    // Mine count overlay on button
    mineCountText.setFont(font);
    mineCountText.setCharacterSize(12);
    mineCountText.setFillColor(sf::Color::White);

    buildButtons();
}

// ── Build structure buttons ───────────────────────────────────────────────────
void HUD::buildButtons() {
    
    struct ButtonDef {
        TowerType   type;
        std::string name;
        sf::Color   color;
    };

    std::vector<ButtonDef> defs = {
        { TowerType::WaterTower, "Water\nTower", sf::Color(30, 100, 200)  },
        { TowerType::SunBeam,    "Sun\nBeam",    sf::Color(200, 160, 30)  },
        { TowerType::TreeTower,  "Tree\nTower",  sf::Color(34, 120, 34)   },
        { TowerType::WaterMine,  "Water\nMine",  sf::Color(0, 130, 130)   },
    };

    float totalWidth = defs.size() * BTN_WIDTH + (defs.size() - 1) * BTN_PADDING;
    float startX     = (windowWidth - totalWidth) / 2.f;
    float y          = windowHeight - BTN_HEIGHT - BTN_Y_OFFSET;

    for (int i = 0; i < (int)defs.size(); i++) {
        TowerButton btn;
        btn.type = defs[i].type;
        int cost = Tower::getCost(defs[i].type);

        float x = startX + i * (BTN_WIDTH + BTN_PADDING);

        btn.shape.setSize({ BTN_WIDTH, BTN_HEIGHT });
        btn.shape.setPosition(x, y);
        btn.shape.setFillColor(defs[i].color);
        btn.shape.setOutlineColor(sf::Color::White);
        btn.shape.setOutlineThickness(1.5f);

        btn.nameText.setFont(font);
        btn.nameText.setString(defs[i].name);
        btn.nameText.setCharacterSize(14);
        btn.nameText.setFillColor(sf::Color::White);
        btn.nameText.setPosition(x + 8.f, y + 6.f);

        btn.costText.setFont(font);
        btn.costText.setString("$" + std::to_string(cost));
        btn.costText.setCharacterSize(13);
        btn.costText.setFillColor(sf::Color(200, 230, 255));
        btn.costText.setPosition(x + 8.f, y + BTN_HEIGHT - 22.f);

        btn.selected = false;

        buttons.push_back(btn);
    }
}

// ── Update ────────────────────────────────────────────────────────────────────
void HUD::update(float dt, int waterPoints, int waveNumber, bool newWave, int restoredCount, int totalCornucopias, int mineCount, int mineCap) {
    this->waterPoints      = waterPoints;
    this->waveNumber       = waveNumber;
    this->restoredCount    = restoredCount;
    this->totalCornucopias = totalCornucopias;
    this->mineCount        = mineCount;
    this->mineCap          = mineCap;

    if (newWave) announcementTimer = 2.5f;
    if (announcementTimer > 0.f) announcementTimer -= dt;

    tickDayNight(dt);
}

// ── Day/Night cycle ───────────────────────────────────────────────────────────
void HUD::tickDayNight(float dt) {
    _cycleCompleted = false;  // reset every frame - only true for one frame

    dayNightProgress += dt / (_isNight ? nightDuration : dayDuration);

    if (dayNightProgress >= 1.f) {
        dayNightProgress = 0.f;
        _isNight = !_isNight;

        // Trigger a new wave when night begins
        if (_isNight)
            _cycleCompleted = true;
    }

    // Bar color changes between day (yellow) and night (dark blue)
    sf::Color barColor = _isNight
        ? sf::Color(50, 50, 150)
        : sf::Color(240, 200, 50);

    dayNightBar.setSize({ 200.f * dayNightProgress, 18.f });
    dayNightBar.setFillColor(barColor);
    dayNightLabel.setString(_isNight ? "Night" : "Day");
}

bool HUD::isNight()            const { return _isNight; }
bool HUD::cycleJustCompleted()       { return _cycleCompleted; }

void HUD::reset() {
    dayNightProgress  = 0.f;
    _isNight          = false;
    _cycleCompleted   = false;
    announcementTimer = 0.f;
    waveNumber        = 0;
    dayNightBar.setSize({ 0.f, 18.f });
}

// ── Handle button clicks ──────────────────────────────────────────────────────
bool HUD::handleClick(sf::Vector2f mousePos) {
    for (auto& btn : buttons) {
        if (btn.type == TowerType::WaterMine && mineCount >= mineCap) continue;
        if (btn.shape.getGlobalBounds().contains(mousePos)) {
            if (btn.selected) {
                deselect();
            } else {
                for (auto& b : buttons) {
                    b.selected = false;
                    b.shape.setOutlineThickness(1.5f);
                }
                btn.selected = true;
                btn.shape.setOutlineThickness(3.f);
                selectedTower = btn.type;
                _hasSelection = true;
            }
            return true;
        }
    }
    return false;
}

void HUD::deselect() {
    for (auto& b : buttons) {
        b.selected = false;
        b.shape.setOutlineThickness(1.5f);
    }
    _hasSelection = false;
}

bool      HUD::hasSelection()     const { return _hasSelection; }
TowerType HUD::getSelectedTower() const { return selectedTower; }


// ── Draw ──────────────────────────────────────────────────────────────────────
void HUD::draw(sf::RenderWindow& window) {
    drawDayNightBar(window);
    drawCurrency(window);
    drawWaveInfo(window);
    drawStructureButtons(window);
    drawCornucopiaCount(window);
}

void HUD::drawDayNightBar(sf::RenderWindow& window) {
    window.draw(dayNightBg);
    window.draw(dayNightBar);
    window.draw(dayNightLabel);
}

void HUD::drawCurrency(sf::RenderWindow& window) {
    std::ostringstream ss;
    ss << "Water: " << waterPoints;
    currencyText.setString(ss.str());

    float textWidth = currencyText.getLocalBounds().width;
    currencyText.setPosition(windowWidth - textWidth - 20.f, 14.f);
    window.draw(currencyText);
}


void HUD::drawWaveInfo(sf::RenderWindow& window) {
    // hidden before the first wave begins
    if (waveNumber > 0) {
        std::ostringstream ss;
        ss << "Wave " << waveNumber;
        waveText.setString(ss.str());
        float textWidth = waveText.getLocalBounds().width;
        waveText.setPosition((windowWidth - textWidth) / 2.f, 14.f);
        window.draw(waveText);
    }

    // Big announcement that fades out
    if (announcementTimer > 0.f) {
        std::ostringstream as;
        as << "Wave " << waveNumber << "!";
        waveAnnouncement.setString(as.str());

        // Fade out during the last 1 second
        int alpha = (announcementTimer < 1.f)
            ? (int)(255 * announcementTimer)
            : 255;
        waveAnnouncement.setFillColor(sf::Color(255, 215, 0, alpha));

        float w = waveAnnouncement.getLocalBounds().width;
        waveAnnouncement.setPosition((windowWidth - w) / 2.f, 80.f);
        window.draw(waveAnnouncement);
    }
}

void HUD::drawStructureButtons(sf::RenderWindow& window) {
    for (auto& btn : buttons) {
        window.draw(btn.shape);
        window.draw(btn.nameText);
        window.draw(btn.costText);

        if (btn.type == TowerType::WaterMine) {
            sf::Vector2f pos  = btn.shape.getPosition();
            sf::Vector2f size = btn.shape.getSize();

            // Mine count label (bottom-right of button)
            mineCountText.setString(std::to_string(mineCount) + "/" + std::to_string(mineCap));
            mineCountText.setPosition(pos.x + size.x - mineCountText.getLocalBounds().width - 6.f,
                                      pos.y + size.y - 22.f);
            window.draw(mineCountText);

            // Dark overlay when at cap
            if (mineCount >= mineCap) {
                sf::RectangleShape overlay(size);
                overlay.setPosition(pos);
                overlay.setFillColor(sf::Color(0, 0, 0, 160));
                window.draw(overlay);
            }
        }
    }
}

void HUD::drawCornucopiaCount(sf::RenderWindow& window) {
    cornucopiaText.setString("Active Cornucopias: " + std::to_string(restoredCount) + " / " + std::to_string(totalCornucopias));
    window.draw(cornucopiaText);
}