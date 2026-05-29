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

    //Window Background Sprites
    menuBgTexture.loadFromFile("assets/OASIS-GRAPHICS/MENU-CUTSCENE_GRAPHICS/2.png");
    menuBgSprite.setTexture(menuBgTexture);
    
    winBgTexture.loadFromFile("assets/OASIS-GRAPHICS/WIN_SCREEN.png");
    winBgSprite.setTexture(winBgTexture);

    loseBgTexture.loadFromFile("assets/OASIS-GRAPHICS/LOSE_SCREEN.png");
    loseBgSprite.setTexture(loseBgTexture);

    dayBgTexture.loadFromFile("assets/OASIS-GRAPHICS/BACKGROUND_MAIN.png");
    dayBgSprite.setTexture(dayBgTexture);

    menuBgSprite.setScale(
        1280.f / menuBgTexture.getSize().x,
        720.f / menuBgTexture.getSize().y
    );

    AssetLoader::loadAll();
    stats.load("assets/stats.txt");

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
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Enter) {
                AudioManager::get().play("flute_start");
                AudioManager::get().playMusic("assets/audio/OasisCutscene.ogg", false);
                cutscene.reset();
                state = GameState::Cutscene;
            } else if (event.key.code == sf::Keyboard::S) {
                state = GameState::StatsMenu;
            }
        }
        return;
    }

    if (state == GameState::StatsMenu) {
        if (event.type == sf::Event::KeyPressed &&
            (event.key.code == sf::Keyboard::Escape ||
             event.key.code == sf::Keyboard::Enter))
            state = GameState::Menu;
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
                AudioManager::get().play("click_tower", 50.f);
                break;
            }
        }
        if (sellTowerIdx == -1) {
            for (int i = 0; i < (int)waterMines.size(); i++) {
                if (dist(waterMines[i].getPosition(), mousePos) < 14.f) {
                    sellMineIdx   = i;
                    sf::Vector2f p = waterMines[i].getPosition();
                    sellPopupRect = { p.x - 32.f, p.y - 58.f, 64.f, 42.f };
                    AudioManager::get().play("click_tower", 50.f);
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
                            stats.oasesRestored++;
                            stats.save("assets/stats.txt");
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
                        AudioManager::get().play("click_tower", 50.f);
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
                        AudioManager::get().play("click_tower", 50.f);
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

    if (activeCount == total) {
        stats.wins++;
        stats.highestWave = std::max(stats.highestWave, waveNumber);
        stats.save("assets/stats.txt");
        state = GameState::Won;
        AudioManager::get().stopMusic();
        AudioManager::get().play("win_sound", 100.f);
        return;
    }
    if (activeCount == 0){
        stats.losses++;
        stats.highestWave = std::max(stats.highestWave, waveNumber);
        stats.save("assets/stats.txt");
        state = GameState::Lost;
        AudioManager::get().stopMusic();
        AudioManager::get().play("lose_sound", 100.f);
        return;
    }

    AudioManager::get().update();
    hud.update(dt, waterPoints, waveNumber, false, activeCount, total, (int)waterMines.size(), mineLimit());

    bool nowNight = hud.isNight();
    //fade
    float targetAlpha = hud.isNight() ? 75.f : 0.f;
    darknessAlpha += (targetAlpha - darknessAlpha) * 1.f * dt;

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
        e.update(dt, nearestTargetPos(e.getPosition()), map);
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
                stats.enemiesKilled++;
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
        window.draw(menuBgSprite);

        window.setHUDView();
        float centerX = windowWidth / 2.f;
        drawText(window, font, "Press ENTER to start", centerX, 400.f, 28, sf::Color(180,180,180), true);
        drawText(window, font, "S - Statistics",       centerX, 450.f, 20, sf::Color(130,180,130), true);
        drawText(window, font, "Version 0.1",          centerX, 530.f, 18, sf::Color(100,100,100), true);
    }
    else if (state == GameState::StatsMenu) {
        window.draw(menuBgSprite);
        window.setHUDView();

        // Dark panel
        float pw = 480.f, ph = 360.f;
        float px = (windowWidth  - pw) / 2.f;
        float py = (windowHeight - ph) / 2.f;
        sf::RectangleShape panel({ pw, ph });
        panel.setPosition(px, py);
        panel.setFillColor(sf::Color(8, 12, 28, 230));
        panel.setOutlineColor(sf::Color(255, 215, 0, 200));
        panel.setOutlineThickness(2.f);
        window.draw(panel);

        // Divider under title
        float divY = py + 54.f;
        sf::RectangleShape divider({ pw - 40.f, 1.f });
        divider.setPosition(px + 20.f, divY);
        divider.setFillColor(sf::Color(255, 215, 0, 120));
        window.draw(divider);

        float cx   = windowWidth / 2.f;
        float rowX = px + 60.f;
        float valX = px + pw - 60.f;
        float rowY = divY + 24.f;
        float rowH = 46.f;

        sf::Color gold (255, 215,   0);
        sf::Color label(200, 200, 200);
        sf::Color value(180, 230, 255);
        sf::Color dim  (120, 120, 120);

        drawText(window, font, "STATISTICS", cx, py + 16.f, 30, gold, true);

        // Row helper: label left, value right
        struct Row { const char* lbl; int val; };
        Row rows[] = {
            { "Wins",           stats.wins          },
            { "Losses",         stats.losses        },
            { "Enemies Killed", stats.enemiesKilled },
            { "Oases Restored", stats.oasesRestored },
            { "Highest Wave",   stats.highestWave   },
        };
        for (auto& r : rows) {
            drawText(window, font, r.lbl,                    rowX, rowY, 20, label, false);
            drawText(window, font, std::to_string(r.val),    valX, rowY, 20, value, true);
            rowY += rowH;
        }

        // Bottom divider
        sf::RectangleShape divider2({ pw - 40.f, 1.f });
        divider2.setPosition(px + 20.f, py + ph - 46.f);
        divider2.setFillColor(sf::Color(255, 215, 0, 80));
        window.draw(divider2);

        drawText(window, font, "ESC / ENTER to go back", cx, py + ph - 34.f, 16, dim, true);
    }
    else if (state == GameState::Playing || state == GameState::Paused) {
        window.setWorldView();

        window.scaleAndCenterSprite(dayBgSprite);
        window.draw(dayBgSprite);
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

        //if night fade
        sf::RectangleShape darkness(
        sf::Vector2f((float)windowWidth, (float)windowHeight)
        );

        sf::View currentView = window._worldView; 
        darkness.setSize(currentView.getSize());
        darkness.setPosition(currentView.getCenter() - currentView.getSize() / 2.f);

        darkness.setFillColor(
        sf::Color(43, 16, 26, (sf::Uint8)darknessAlpha)
        );

window.draw(darkness);

        window.draw(darkness);
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
        window.scaleAndCenterSprite(winBgSprite);
        window.draw(winBgSprite);
       
    }

    else if (state == GameState::Lost) {
        window.scaleAndCenterSprite(loseBgSprite);
        window.draw(loseBgSprite);
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
