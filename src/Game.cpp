#include "Game.h"
#include "Window.h"
#include "AudioManager.h"
#include "AssetLoader.h"
#include "GameUtils.h"
#include <limits>
#include <algorithm> 

Game::Game(int width, int height)
    : windowWidth(width)
    , windowHeight(height)
    , state(GameState::Menu)
    , map()
    , spawner(width, height)
    , hud(font, width, height)
    , cutscene(font, width, height)
    , waterPoints(150)
    , waveNumber(0)
    , hoveredTile(-1, -1)
{
    font.loadFromFile("assets/fonts/Minecraft.otf");
    map.loadFromFile("assets/map.txt");

    

    AssetLoader::loadAll();

    placeCornucopias();
}

/* ---------------------------------------------------------
    EVENTS
------------------------------------------------------------ */

void Game::processEvent(const sf::Event& event, Window& window) {
    if (event.type == sf::Event::Closed) {
        window.close();
        return;
    }

    if (state == GameState::Menu) {
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Enter) {

            AudioManager::get().play("flute_start");
            AudioManager::get().playMusic("assets/audio/OasisCutscene.ogg", false);
            cutscene.reset();
            state = GameState::Cutscene;

        }
        return;
    }

    if (state == GameState::Cutscene) {
        if (event.type == sf::Event::KeyPressed) {
            cutscene.skip();
            AudioManager::get().stopAllSounds();
            AudioManager::get().playMusic("assets/audio/OasisDay.ogg");
        }
        return;
    }

    if (state == GameState::Won || state == GameState::Lost) {
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Enter)
            reset();
        return;
    }

    if (state == GameState::Paused) {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape)
                state = GameState::Playing;
            else if (event.key.code == sf::Keyboard::M)
                reset();
        }
        return;
    }

    if (state == GameState::Playing &&
        event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape)
    {
        state = GameState::Paused;
        return;
    }

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f worldPos = window.mapPixelToCoords(
            { event.mouseMove.x, event.mouseMove.y }); // uses world view
        hoveredTile = map.worldToGrid(worldPos);
    }

    if (state == GameState::Playing &&
        event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Right)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(
            { event.mouseButton.x, event.mouseButton.y }); // world view
        selectedCornIdx = -1;
        sellTowerIdx = -1;
        sellMineIdx  = -1;
        for (auto& t : towers) t.setShowRange(false);
        for (int i = 0; i < (int)towers.size(); i++) {
            if (dist(towers[i].getPosition(), mousePos) < 14.f) {
                sellTowerIdx  = i;
                towers[i].setShowRange(true);
                sf::Vector2f p = towers[i].getPosition();
                sellPopupRect = { p.x - 32.f, p.y - 58.f, 64.f, 42.f };
                break;
            }
        }
        if (sellTowerIdx == -1) {
            for (int i = 0; i < (int)waterMines.size(); i++) {
                if (dist(waterMines[i].getPosition(), mousePos) < 14.f) {
                    sellMineIdx   = i;
                    sf::Vector2f p = waterMines[i].getPosition();
                    sellPopupRect = { p.x - 32.f, p.y - 58.f, 64.f, 42.f };
                    break;
                }
            }
        }

        if (sellTowerIdx == -1 && sellMineIdx == -1)
            hud.deselect();
    }

    if (state == GameState::Playing &&
        event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(
            { event.mouseButton.x, event.mouseButton.y }); // world view
        sf::Vector2f hudPos = window.mapMouseToHUD(
            { event.mouseButton.x, event.mouseButton.y }); // HUD (virtual 1280x720)
        if (hud.handleClick(hudPos)) {
            for (auto& t : towers) t.setShowRange(false);
            sellTowerIdx   = -1;
            sellMineIdx    = -1;
            selectedCornIdx = -1;
        } else {
            bool handled = false;

            // Sell popup: confirm or dismiss
            if (sellTowerIdx >= 0 || sellMineIdx >= 0) {
                if (sellPopupRect.contains(mousePos)) {
                    if (sellTowerIdx >= 0 && sellTowerIdx < (int)towers.size()) {
                        int refund = Tower::getCost(towers[sellTowerIdx].getType()) / 2;
                        waterPoints += refund;
                        map.clearTower(map.worldToGrid(towers[sellTowerIdx].getPosition()));
                        towers.erase(towers.begin() + sellTowerIdx);
                    } else if (sellMineIdx >= 0 && sellMineIdx < (int)waterMines.size()) {
                        int refund = Tower::getCost(TowerType::WaterMine) / 2;
                        waterPoints += refund;
                        map.clearTower(map.worldToGrid(waterMines[sellMineIdx].getPosition()));
                        waterMines.erase(waterMines.begin() + sellMineIdx);
                    }
                    handled = true;
                }
                for (auto& t : towers) t.setShowRange(false);
                sellTowerIdx = -1;
                sellMineIdx  = -1;
            }

            // Step 2: confirm restore via open popup
            for (auto& c : cornucopias) {
                if (c.isBroken() && c.isPopupOpen()) {
                    if (c.getPopupBounds().contains(mousePos)) {
                        int activeCount = 0;
                        for (const auto& cc : cornucopias) if (cc.isActive()) activeCount++;
                        int cost = Cornucopia::getRestoreCost(activeCount);
                        if (waterPoints >= cost) {
                            waterPoints -= cost;
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

            // Soldier ordering: if a cornucopia is selected, click inside radius moves soldier
            if (!handled && selectedCornIdx >= 0) {
                sf::Vector2f cornPos = cornucopias[selectedCornIdx].getPosition();
                if (dist(mousePos, cornPos) <= Soldier::PATROL_RADIUS) {
                    cornucopias[selectedCornIdx].orderSoldierTo(mousePos);
                    handled = true;
                } else {
                    selectedCornIdx = -1;
                }
            }

            // Cornucopia selection: click an active cornucopia to show its patrol radius
            if (!handled) {
                for (int i = 0; i < (int)cornucopias.size(); i++) {
                    if (cornucopias[i].isActive() && cornucopias[i].containsPoint(mousePos)) {
                        selectedCornIdx = (selectedCornIdx == i) ? -1 : i;
                        hud.deselect();
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
                    if (map.isPlaceable(tile) && hud.hasSelection()) {
                        handlePlacement(tile);
                    } else {
                        // Click on an existing tower or mine deselects the HUD
                        for (const auto& t : towers) {
                            if (dist(t.getPosition(), mousePos) < 14.f) {
                                hud.deselect();
                                break;
                            }
                        }
                        for (const auto& wm : waterMines) {
                            if (dist(wm.getPosition(), mousePos) < 14.f) {
                                hud.deselect();
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

/* ---------------------------------------------------------
    UPDATE
------------------------------------------------------------ */

void Game::update(float dt) {
    if (state == GameState::Cutscene) {
        cutscene.update(dt);
        if (cutscene.isDone()) {
            state = GameState::Playing;
        }
        return;
    }

    if (state != GameState::Playing) return;

    int activeCount = 0;
    int total       = (int)cornucopias.size();
    for (const auto& c : cornucopias)
        if (c.isActive()) activeCount++;

    if (activeCount == total) { state = GameState::Won;  return; }
    if (activeCount == 0)     { state = GameState::Lost; return; }

    AudioManager::get().update();
    hud.update(dt, waterPoints, waveNumber, false, activeCount, total, (int)waterMines.size(), mineLimit());

    bool nowNight = hud.isNight();
    if (nowNight != wasNight) {
        wasNight = nowNight;
        if (nowNight)
            AudioManager::get().playMusic("assets/audio/OasisNight.ogg", false);
        else
            AudioManager::get().playMusic("assets/audio/OasisDay.ogg");
    }
    if (hud.cycleJustCompleted()) {
        waveNumber++;
        hud.update(0.f, waterPoints, waveNumber, true, activeCount, total, (int)waterMines.size(), mineLimit());
    }

    for (auto& c : cornucopias) c.update(dt);
    for (auto& c : cornucopias) if (c.isActive()) c.updateSoldier(dt, enemies);
    if (selectedCornIdx >= 0 && !cornucopias[selectedCornIdx].isActive())
        selectedCornIdx = -1;

    for (auto& t : towers)      t.update(dt, enemies, bullets);
    for (auto& wm : waterMines) wm.update(dt);

    // Bullet-enemy collision
    for (auto& b : bullets) {
        b.update(dt);
        if (b.isExpired()) continue;
        for (auto& e : enemies) {
            if (!e.isAlive()) continue;
            if (dist(b.getPosition(), e.getPosition()) < 12.f) {
                if (b.getAoeRadius() > 0.f) {
                    for (auto& ae : enemies) {
                        if (!ae.isAlive()) continue;
                        if (dist(b.getPosition(), ae.getPosition()) <= b.getAoeRadius()) {
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

void Game::render(Window& window) {
    if (state == GameState::Cutscene) {
        window.setHUDView();
        cutscene.render((sf::RenderWindow&)window);
        return;
    }

    if (state == GameState::Menu) {
        window.setHUDView();
        float centerX = windowWidth / 2.f;
        drawText(window, font, "OASIS",                centerX, 200.f, 72, sf::Color::White,       true);
        drawText(window, font, "Press ENTER to start", centerX, 340.f, 28, sf::Color(180,180,180), true);
        drawText(window, font, "Version 0.1",          centerX, 500.f, 18, sf::Color(100,100,100), true);
    }
    else if (state == GameState::Playing || state == GameState::Paused) {
        window.setWorldView();
        bool showHover = hud.hasSelection() && !isAdjacentToCornucopia(hoveredTile);
        map.draw(window, showHover ? hoveredTile : sf::Vector2i(-1, -1));

        if (selectedCornIdx >= 0 && selectedCornIdx < (int)cornucopias.size())
            cornucopias[selectedCornIdx].drawSoldierRadius(window);

        int activeCount = 0;
        for (const auto& c : cornucopias) if (c.isActive()) activeCount++;

        for (auto& c : cornucopias) {
            c.draw(window);
            if (c.isBroken()) {
                sf::FloatRect pb = c.getPopupBounds();
                if (c.isPopupOpen()) {
                    int cost = Cornucopia::getRestoreCost(activeCount);
                    drawText(window, font, "Restore?", pb.left + pb.width / 2.f, pb.top + 4.f,
                             13, sf::Color(255, 215, 0), true);
                    drawText(window, font, std::to_string(cost) + " WP",
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
        for (auto& c : cornucopias) c.drawSoldier(window);
        for (auto& e : enemies)     e.draw(window);
        for (auto& b : bullets)     b.draw(window);

        // Sell popup
        if ((sellTowerIdx >= 0 && sellTowerIdx < (int)towers.size()) ||
            (sellMineIdx  >= 0 && sellMineIdx  < (int)waterMines.size()))
        {
            int refund = (sellTowerIdx >= 0)
                ? Tower::getCost(towers[sellTowerIdx].getType()) / 2
                : Tower::getCost(TowerType::WaterMine) / 2;

            sf::RectangleShape popup({ sellPopupRect.width, sellPopupRect.height });
            popup.setPosition(sellPopupRect.left, sellPopupRect.top);
            popup.setFillColor(sf::Color(20, 20, 40, 220));
            popup.setOutlineColor(sf::Color(200, 200, 255, 180));
            popup.setOutlineThickness(1.f);
            window.draw(popup);

            float cx = sellPopupRect.left + sellPopupRect.width / 2.f;
            drawText(window, font, "Sell?",
                     cx, sellPopupRect.top + 4.f,  13, sf::Color(255, 180, 80),  true);
            drawText(window, font, "+" + std::to_string(refund) + " WP",
                     cx, sellPopupRect.top + 22.f, 13, sf::Color(180, 230, 255), true);
        }

        window.setHUDView();
        hud.draw(window);

        if (state == GameState::Paused) {
            sf::RectangleShape overlay(sf::Vector2f((float)windowWidth, (float)windowHeight));
            overlay.setFillColor(sf::Color(0, 0, 0, 160));
            window.draw(overlay);

            float cx = windowWidth / 2.f;
            drawText(window, font, "PAUSED",           cx, 180.f, 64, sf::Color::White,        true);
            drawText(window, font, "ESC - Resume",     cx, 290.f, 28, sf::Color(180, 180, 180), true);
            drawText(window, font, "M - Main Menu",    cx, 340.f, 28, sf::Color(180, 180, 180), true);
        }
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
    waterPoints     = 150;
    waveNumber      = 0;
    wasNight        = false;
    selectedCornIdx = -1;

    map = Map();
    map.loadFromFile("assets/map.txt");
    hud.reset();

    placeCornucopias();

    state = GameState::Menu;
    AudioManager::get().playMusic("assets/audio/OasisTheme.ogg");
}

int Game::mineLimit() const {
    int active = 0;
    for (const auto& c : cornucopias) if (c.isActive()) active++;
    return 5 + active;
}

bool Game::isAdjacentToCornucopia(sf::Vector2i tile) const {
    for (const auto& c : cornucopias) {
        sf::Vector2i ct = map.worldToGrid(c.getPosition());
        if (std::abs(tile.x - ct.x) <= 1 && std::abs(tile.y - ct.y) <= 1)
            return true;
    }
    return false;
}

void Game::handlePlacement(sf::Vector2i tile) {
    TowerType type = hud.getSelectedTower();
    int       cost = Tower::getCost(type);
    if (waterPoints < cost) return;
    if (isAdjacentToCornucopia(tile)) return;
    if (type == TowerType::WaterMine && (int)waterMines.size() >= mineLimit()) return;

    waterPoints -= cost;
    map.setTower(tile);

    if (type == TowerType::WaterMine) {
        waterMines.emplace_back(map.tileCenter(tile), font);
    } else {
        towers.push_back(Tower(map.tileCenter(tile), type));
    }

    AudioManager::get().play("construction");
}

Game::NearestTarget Game::findNearestTarget(sf::Vector2f from) const {
    NearestTarget result;
    float minDist = std::numeric_limits<float>::max();

    for (int i = 0; i < (int)cornucopias.size(); i++) {
        if (!cornucopias[i].isActive()) continue;
        float d = dist(cornucopias[i].getPosition(), from);
        if (d < minDist) { minDist = d; result = {i, -1, -1}; }
    }
    for (int i = 0; i < (int)towers.size(); i++) {
        if (towers[i].isDestroyed()) continue;
        float d = dist(towers[i].getPosition(), from);
        if (d < minDist) { minDist = d; result = {-1, i, -1}; }
    }
    for (int i = 0; i < (int)waterMines.size(); i++) {
        if (waterMines[i].isDestroyed()) continue;
        float d = dist(waterMines[i].getPosition(), from);
        if (d < minDist) { minDist = d; result = {-1, -1, i}; }
    }
    return result;
}

sf::Vector2f Game::nearestTargetPos(sf::Vector2f from) const {
    NearestTarget r = findNearestTarget(from);
    if (r.cornIdx >= 0) return cornucopias[r.cornIdx].getPosition();
    if (r.towIdx  >= 0) return towers[r.towIdx].getPosition();
    if (r.wmIdx   >= 0) return waterMines[r.wmIdx].getPosition();
    return from;
}

void Game::damageNearestTarget(sf::Vector2f from, float damage) {
    NearestTarget r = findNearestTarget(from);
    if (r.cornIdx >= 0) cornucopias[r.cornIdx].takeDamage(damage);
    else if (r.towIdx >= 0) towers[r.towIdx].takeDamage(damage);
    else if (r.wmIdx  >= 0) waterMines[r.wmIdx].takeDamage(damage);
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
