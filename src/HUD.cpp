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

    buildButtons();
}

// ── Build structure buttons ───────────────────────────────────────────────────
void HUD::buildButtons() {
    struct ButtonDef {
        SelectedTower type;
        std::string   name;
        int           cost;
        sf::Color     color;
    };

    std::vector<ButtonDef> defs = {
        { SelectedTower::WaterTower, "Water\nTower",  50,  sf::Color(30, 100, 200)  },
        { SelectedTower::SunBeam,    "Sun\nBeam",     100, sf::Color(200, 160, 30)  },
        { SelectedTower::TreeTower,  "Tree\nTower",   75,  sf::Color(34, 120, 34)   },
        { SelectedTower::Cornucopia, "Cornucopia",    200, sf::Color(160, 110, 10)  },
    };

    // Center the row at the bottom middle of the screen
    float totalWidth = defs.size() * BTN_WIDTH + (defs.size() - 1) * BTN_PADDING;
    float startX     = (windowWidth - totalWidth) / 2.f;
    float y          = windowHeight - BTN_HEIGHT - BTN_Y_OFFSET;

    for (int i = 0; i < (int)defs.size(); i++) {
        TowerButton btn;
        btn.type = defs[i].type;
        btn.cost = defs[i].cost;

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
        btn.costText.setString("$" + std::to_string(defs[i].cost));
        btn.costText.setCharacterSize(13);
        btn.costText.setFillColor(sf::Color(200, 230, 255));
        btn.costText.setPosition(x + 8.f, y + BTN_HEIGHT - 22.f);

        // First button selected by default
        btn.selected = (i == 0);

        buttons.push_back(btn);
    }
}

// ── Update ────────────────────────────────────────────────────────────────────
void HUD::update(float dt, int waterPoints, int waveNumber, bool newWave, int cornucopiaCount) {
    this->waterPoints     = waterPoints;
    this->waveNumber      = waveNumber;
    this->cornucopiaCount = cornucopiaCount;

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

        // A full cycle completes when we flip back to day
        if (!_isNight)
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

// ── Handle button clicks ──────────────────────────────────────────────────────
bool HUD::handleClick(sf::Vector2f mousePos) {
    for (auto& btn : buttons) {
        if (btn.shape.getGlobalBounds().contains(mousePos)) {
            for (auto& b : buttons) {
                b.selected = false;
                b.shape.setOutlineThickness(1.5f);
            }
            btn.selected = true;
            btn.shape.setOutlineThickness(3.f);
            selectedTower = btn.type;
            return true;
        }
    }
    return false;
}

SelectedTower HUD::getSelectedTower() const { return selectedTower; }

int HUD::getSelectedCost() const {
    for (const auto& btn : buttons)
        if (btn.type == selectedTower) return btn.cost;
    return 0;
}

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
    // Small persistent label
    std::ostringstream ss;
    ss << "Wave " << waveNumber;
    waveText.setString(ss.str());
    float textWidth = waveText.getLocalBounds().width;
    waveText.setPosition((windowWidth - textWidth) / 2.f, 14.f);
    window.draw(waveText);

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
    }
}

void HUD::drawCornucopiaCount(sf::RenderWindow& window) {
    cornucopiaText.setString("Oasis: " + std::to_string(cornucopiaCount) + " / 5");
    window.draw(cornucopiaText);
}