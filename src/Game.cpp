#include "Game.h"
#include <cmath>
#include <limits>
#include <algorithm>

Game::Game(int width, int height)
    : windowWidth(width)
    , windowHeight(height)
    , state(GameState::Menu)
    , map()
    , spawner(width, height)
    , hud(font, width, height)
    , waterPoints(150)
    , waveNumber(0)
    , hoveredTile(-1, -1)
{
    font.loadFromFile("assets/fonts/desert_road/Desert_Road.otf");
    map.loadFromFile("assets/map.txt");

    placeCornucopias();
}

/* ---------------------------------------------------------
    EVENTS
------------------------------------------------------------ */

void Game::processEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::Closed) {
        window.close();
        return;
    }

    if (state == GameState::Menu) {
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Enter)
            state = GameState::Playing;
        return;
    }

    if (state == GameState::Won || state == GameState::Lost) {
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Enter)
            reset();
        return;
    }

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f worldPos = window.mapPixelToCoords(
            { event.mouseMove.x, event.mouseMove.y });
        hoveredTile = map.worldToGrid(worldPos);
    }

    if (state == GameState::Playing &&
        event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(
            { event.mouseButton.x, event.mouseButton.y });
        if (!hud.handleClick(mousePos)) {
            bool handled = false;

            // Step 2: confirm restore via open popup
            for (auto& c : cornucopias) {
                if (c.isBroken() && c.isPopupOpen()) {
                    if (c.getPopupBounds().contains(mousePos)) {
                        if (waterPoints >= Cornucopia::RESTORE_COST) {
                            waterPoints -= Cornucopia::RESTORE_COST;
                            c.restore();
                        }
                    } else {
                        c.closePopup();
                    }
                    handled = true;
                    break;
                }
            }

            if (!handled) {
                // Step 1: click broken cornucopia to open popup
                for (auto& c : cornucopias) {
                    if (c.isBroken() && c.containsPoint(mousePos)) {
                        for (auto& other : cornucopias) other.closePopup();
                        c.openPopup();
                        handled = true;
                        break;
                    }
                }
            }

            if (!handled) {
                // Close any open popup when clicking elsewhere
                for (auto& c : cornucopias) c.closePopup();

                bool collected = false;
                for (auto& wm : waterMines) {
                    if (wm.isReady() && wm.contains(mousePos)) {
                        waterPoints += wm.collect();
                        collected = true;
                        break;
                    }
                }
                if (!collected) {
                    sf::Vector2i tile = map.worldToGrid(mousePos);
                    if (map.isPlaceable(tile))
                        handlePlacement(tile);
                }
            }
        }
    }
}

/* ---------------------------------------------------------
    UPDATE
------------------------------------------------------------ */

void Game::update(float dt) {
    if (state != GameState::Playing) return;

    int activeCount = 0;
    int total       = (int)cornucopias.size();
    for (const auto& c : cornucopias)
        if (c.isActive()) activeCount++;

    if (activeCount == total) { state = GameState::Won;  return; }
    if (activeCount == 0)     { state = GameState::Lost; return; }

    hud.update(dt, waterPoints, waveNumber, false, activeCount, total);
    if (hud.cycleJustCompleted()) {
        waveNumber++;
        hud.update(dt, waterPoints, waveNumber, true, activeCount, total);
    }

    for (auto& c : cornucopias) c.update(dt);
    for (auto& t : towers)      t.update(dt, enemies, bullets);
    for (auto& wm : waterMines) wm.update(dt);

    // Bullet-enemy collision
    for (auto& b : bullets) {
        b.update(dt);
        if (b.isExpired()) continue;
        for (auto& e : enemies) {
            if (!e.isAlive()) continue;
            sf::Vector2f d    = b.getPosition() - e.getPosition();
            float        dist = std::sqrt(d.x*d.x + d.y*d.y);
            if (dist < 16.f) {
                if (b.getAoeRadius() > 0.f) {
                    for (auto& ae : enemies) {
                        if (!ae.isAlive()) continue;
                        sf::Vector2f ad    = b.getPosition() - ae.getPosition();
                        float        adist = std::sqrt(ad.x*ad.x + ad.y*ad.y);
                        if (adist <= b.getAoeRadius()) {
                            ae.takeDamage(b.getDamage());
                            ae.applySlow(b.getSlowFactor(), b.getSlowDuration());
                        }
                    }
                } else {
                    e.takeDamage(b.getDamage());
                }
                b.expire();
                break;
            }
        }
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return b.isExpired(); }), bullets.end());

    if (hud.isNight()) spawner.update(dt, enemies, waveNumber);

    // Enemy movement and attacks
    for (auto& e : enemies) {
        if (!e.isAlive()) continue;
        e.update(dt, nearestTargetPos(e.getPosition()));
        if (e.consumeAttack())
            damageNearestTarget(e.getPosition(), e.getDamage());
    }

    // Remove destroyed towers and free their tiles
    for (const auto& t : towers)
        if (t.isDestroyed())
            map.clearTower(map.worldToGrid(t.getPosition()));
    towers.erase(std::remove_if(towers.begin(), towers.end(),
        [](const Tower& t) { return t.isDestroyed(); }), towers.end());

    // Remove destroyed water mines and free their tiles
    for (const auto& wm : waterMines)
        if (wm.isDestroyed())
            map.clearTower(map.worldToGrid(wm.getPosition()));
    waterMines.erase(std::remove_if(waterMines.begin(), waterMines.end(),
        [](const WaterMine& wm) { return wm.isDestroyed(); }), waterMines.end());

    // SporePuff splitting
    std::vector<Enemy> toSpawn;
    for (auto& e : enemies) {
        if (!e.isAlive()) {
            if (e.shouldSplit()) {
                sf::Vector2f p  = e.getPosition();
                int          ng = e.getSplitGeneration() + 1;
                toSpawn.emplace_back(EnemyType::SporePuff, p + sf::Vector2f(-12.f, 0.f), ng);
                toSpawn.emplace_back(EnemyType::SporePuff, p + sf::Vector2f( 12.f, 0.f), ng);
            } else {
                waterPoints += e.getReward();
            }
        }
    }
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e) { return !e.isAlive(); }), enemies.end());
    for (auto& s : toSpawn) enemies.push_back(std::move(s));
}

/* ---------------------------------------------------------
    RENDER
------------------------------------------------------------ */

void Game::render(sf::RenderWindow& window) {
    if (state == GameState::Menu) {
        float centerX = windowWidth / 2.f;
        drawText(window, font, "OASIS",                centerX, 200.f, 72, sf::Color::White,       true);
        drawText(window, font, "Press ENTER to start", centerX, 340.f, 28, sf::Color(180,180,180), true);
        drawText(window, font, "Version 0.1",          centerX, 500.f, 18, sf::Color(100,100,100), true);
    }
    else if (state == GameState::Playing) {
        map.draw(window, hoveredTile);
        for (auto& c : cornucopias) {
            c.draw(window);
            if (c.isBroken()) {
                sf::FloatRect pb = c.getPopupBounds();
                if (c.isPopupOpen()) {
                    drawText(window, font, "Restore?", pb.left + pb.width / 2.f, pb.top + 4.f,
                             13, sf::Color(255, 215, 0), true);
                    drawText(window, font, std::to_string(Cornucopia::RESTORE_COST) + " WP",
                             pb.left + pb.width / 2.f, pb.top + 22.f,
                             13, sf::Color(180, 230, 255), true);
                } else {
                    drawText(window, font, "[Click]", c.getPosition().x,
                             c.getPosition().y - 50.f, 11, sf::Color(160, 160, 160), true);
                }
            }
        }
        for (auto& t : towers)      t.draw(window);
        for (auto& wm : waterMines) wm.draw(window);
        for (auto& e : enemies)     e.draw(window);
        for (auto& b : bullets)     b.draw(window);
        hud.draw(window);
    }
    else if (state == GameState::Won) {
        float centerX = windowWidth / 2.f;
        drawText(window, font, "YOU WIN!",                  centerX, 180.f, 72, sf::Color(255,215,0),   true);
        drawText(window, font, "The Oasis is restored.",    centerX, 290.f, 32, sf::Color::White,       true);
        drawText(window, font, "Press ENTER to play again", centerX, 400.f, 24, sf::Color(180,180,180), true);
    }
    else if (state == GameState::Lost) {
        float centerX = windowWidth / 2.f;
        drawText(window, font, "GAME OVER",                  centerX, 180.f, 72, sf::Color(200,50,50),   true);
        drawText(window, font, "The last Oasis has fallen.", centerX, 290.f, 32, sf::Color::White,       true);
        drawText(window, font, "Press ENTER to try again",   centerX, 400.f, 24, sf::Color(180,180,180), true);
    }
}

/* ---------------------------------------------------------
    HELPING FUNCTIONS
------------------------------------------------------------ */

void Game::reset() {
    towers.clear();
    enemies.clear();
    cornucopias.clear();
    bullets.clear();
    waterMines.clear();
    waterPoints = 150;
    waveNumber  = 0;

    map = Map();
    map.loadFromFile("assets/map.txt");

    placeCornucopias();

    state = GameState::Menu;
}

void Game::handlePlacement(sf::Vector2i tile) {
    TowerType type = hud.getSelectedTower();
    int       cost = Tower::getCost(type);
    if (waterPoints < cost) return;

    waterPoints -= cost;
    map.setTower(tile);

    if (type == TowerType::WaterMine) {
        waterMines.emplace_back(map.tileCenter(tile), font);
    } else {
        towers.push_back(Tower(map.tileCenter(tile), type));
    }
}

sf::Vector2f Game::nearestTargetPos(sf::Vector2f from) const {
    sf::Vector2f best    = from;
    float        minDist = std::numeric_limits<float>::max();

    for (const auto& c : cornucopias) {
        if (!c.isActive()) continue;
        sf::Vector2f d    = c.getPosition() - from;
        float        dist = std::sqrt(d.x*d.x + d.y*d.y);
        if (dist < minDist) { minDist = dist; best = c.getPosition(); }
    }
    for (const auto& t : towers) {
        if (t.isDestroyed()) continue;
        sf::Vector2f d    = t.getPosition() - from;
        float        dist = std::sqrt(d.x*d.x + d.y*d.y);
        if (dist < minDist) { minDist = dist; best = t.getPosition(); }
    }
    for (const auto& wm : waterMines) {
        if (wm.isDestroyed()) continue;
        sf::Vector2f d    = wm.getPosition() - from;
        float        dist = std::sqrt(d.x*d.x + d.y*d.y);
        if (dist < minDist) { minDist = dist; best = wm.getPosition(); }
    }
    return best;
}

void Game::damageNearestTarget(sf::Vector2f from, float damage) {
    float minDist = std::numeric_limits<float>::max();
    int   cornIdx = -1;
    int   towIdx  = -1;
    int wmIdx = -1;

    for (int i = 0; i < (int)cornucopias.size(); i++) {
        if (!cornucopias[i].isActive()) continue;
        sf::Vector2f d    = cornucopias[i].getPosition() - from;
        float        dist = std::sqrt(d.x*d.x + d.y*d.y);
        if (dist < minDist) { minDist = dist; cornIdx = i; towIdx = -1; wmIdx = -1; }
    }
    for (int i = 0; i < (int)towers.size(); i++) {
        if (towers[i].isDestroyed()) continue;
        sf::Vector2f d    = towers[i].getPosition() - from;
        float        dist = std::sqrt(d.x*d.x + d.y*d.y);
        if (dist < minDist) { minDist = dist; towIdx = i; cornIdx = -1; wmIdx = -1; }
    }

    for (int i = 0; i < (int)waterMines.size(); i++) {
        if (waterMines[i].isDestroyed()) continue;
        sf::Vector2f d    = waterMines[i].getPosition() - from;
        float        dist = std::sqrt(d.x*d.x + d.y*d.y);
        if (dist < minDist) { minDist = dist; wmIdx = i; cornIdx = -1; towIdx = -1; }
    }

    if (cornIdx >= 0) cornucopias[cornIdx].takeDamage(damage);
    else if (towIdx >= 0) towers[towIdx].takeDamage(damage);
    else if (wmIdx  >= 0) waterMines[wmIdx].takeDamage(damage);
}


void Game::placeCornucopias() {
    static constexpr int MARGIN   = 6;  // min tiles from any edge
    static constexpr int MIN_DIST = 8;  // min tile distance between cornucopias

    // Center cornucopia starts active
    sf::Vector2i centerTile = map.worldToGrid({ windowWidth / 2.f, windowHeight / 2.f });
    cornucopias.emplace_back(map.tileCenter(centerTile));
    cornucopias.back().restore();
    map.setTower(centerTile);

    std::vector<sf::Vector2i> placed = { centerTile };

    // Remaining cornucopias start broken, placed randomly
    while ((int)placed.size() < Cornucopia::NUM_CORNUCOPIAS) {
        sf::Vector2i tile(MARGIN + std::rand() % (GRID_COLS - 2 * MARGIN),
                          MARGIN + std::rand() % (GRID_ROWS - 2 * MARGIN));

        if (!map.isPlaceable(tile)) continue;

        bool tooClose = false;
        for (const auto& p : placed) {
            float dx = (float)(tile.x - p.x);
            float dy = (float)(tile.y - p.y);
            if (std::sqrt(dx*dx + dy*dy) < (float)MIN_DIST) { tooClose = true; break; }
        }
        if (tooClose) continue;

        cornucopias.emplace_back(map.tileCenter(tile));
        map.setTower(tile);
        placed.push_back(tile);
    }
}

void Game::drawText(sf::RenderWindow& window, sf::Font& font,
                    const std::string& str, float x, float y,
                    unsigned int size, sf::Color color, bool centered)
{
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    if (centered) x -= text.getLocalBounds().width / 2.f;
    text.setPosition(x, y);
    window.draw(text);
}
