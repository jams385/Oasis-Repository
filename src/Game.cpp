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
    , waveNumber(1)
    , hoveredTile(-1, -1)
{
    font.loadFromFile("assets/fonts/desert_road/Desert_Road.otf");
    map.loadFromFile("assets/map.txt");

    sf::Vector2i centerTile = map.worldToGrid({ width / 2.f, height / 2.f });
    cornucopias.emplace_back(map.tileCenter(centerTile));
    map.setTower(centerTile);
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
        hoveredTile = map.worldToGrid({ (float)event.mouseMove.x,
                                        (float)event.mouseMove.y });
    }

    if (state == GameState::Playing &&
        event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        if (!hud.handleClick(mousePos)) {
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

/* ---------------------------------------------------------
    UPDATE
------------------------------------------------------------ */

void Game::update(float dt) {
    if (state != GameState::Playing) return;

    int aliveCornucopias = 0;
    for (const auto& c : cornucopias)
        if (!c.isDestroyed()) aliveCornucopias++;

    if (aliveCornucopias >= 5) { state = GameState::Won;  return; }
    if (aliveCornucopias == 0) { state = GameState::Lost; return; }

    hud.update(dt, waterPoints, waveNumber, false, aliveCornucopias);
    if (hud.cycleJustCompleted()) {
        waveNumber++;
        hud.update(dt, waterPoints, waveNumber, true, aliveCornucopias);
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
        e.update(dt, nearestCornucopiaPos(e.getPosition()));
        if (e.consumeAttack()) {
            int idx = nearestCornucopiaIdx(e.getPosition());
            if (idx >= 0) cornucopias[idx].takeDamage(e.getDamage());
        }
    }

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
        for (auto& c : cornucopias) c.draw(window);
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
    waveNumber  = 1;

    map = Map();
    map.loadFromFile("assets/map.txt");

    sf::Vector2i centerTile = map.worldToGrid({ windowWidth / 2.f, windowHeight / 2.f });
    cornucopias.emplace_back(map.tileCenter(centerTile));
    map.setTower(centerTile);

    state = GameState::Menu;
}

void Game::handlePlacement(sf::Vector2i tile) {
    TowerType type = hud.getSelectedTower();
    int       cost = Tower::getCost(type);
    if (waterPoints < cost) return;

    waterPoints -= cost;
    map.setTower(tile);

    if (type == TowerType::Cornucopia) {
        cornucopias.emplace_back(map.tileCenter(tile));
    } else if (type == TowerType::WaterMine) {
        waterMines.emplace_back(map.tileCenter(tile), font);
    } else {
        towers.push_back(Tower(map.tileCenter(tile), type));
    }
}

sf::Vector2f Game::nearestCornucopiaPos(sf::Vector2f from) const {
    sf::Vector2f best    = from;
    float        minDist = std::numeric_limits<float>::max();
    for (const auto& c : cornucopias) {
        if (c.isDestroyed()) continue;
        sf::Vector2f d    = c.getPosition() - from;
        float        dist = std::sqrt(d.x*d.x + d.y*d.y);
        if (dist < minDist) { minDist = dist; best = c.getPosition(); }
    }
    return best;
}

int Game::nearestCornucopiaIdx(sf::Vector2f from) const {
    int   bestIdx = -1;
    float minDist = std::numeric_limits<float>::max();
    for (int i = 0; i < (int)cornucopias.size(); i++) {
        if (cornucopias[i].isDestroyed()) continue;
        sf::Vector2f d    = cornucopias[i].getPosition() - from;
        float        dist = std::sqrt(d.x*d.x + d.y*d.y);
        if (dist < minDist) { minDist = dist; bestIdx = i; }
    }
    return bestIdx;
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
