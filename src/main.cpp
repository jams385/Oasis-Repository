#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include "HUD.h"
#include "Map.h"
#include "Tower.h"
#include "Enemy.h"
#include "EnemySpawner.h"
#include "Cornucopia.h"

using namespace std;

/* read program documentation for explanations and progress documentation:
https://docs.google.com/document/d/1sXYtkZC9QKFjfhb1YHDYnqGQbtqAIGhFicqRt7q9XC8/edit?tab=t.eynqtmny1yz4 */

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

std::vector<Tower>      towers;
std::vector<Enemy>      enemies;
std::vector<Cornucopia> cornucopias;
EnemySpawner            spawner(WINDOW_WIDTH, WINDOW_HEIGHT);

enum class GameState { Menu, Playing, Won, Lost };

sf::Vector2f nearestCornucopia(const std::vector<Cornucopia>& cs, sf::Vector2f from) {
    sf::Vector2f best = from;
    float minDist = std::numeric_limits<float>::max();
    for (const auto& c : cs) {
        if (c.isDestroyed()) continue;
        sf::Vector2f d = c.getPosition() - from;
        float dist = std::sqrt(d.x*d.x + d.y*d.y);
        if (dist < minDist) { minDist = dist; best = c.getPosition(); }
    }
    return best;
}

// Returns the index into `cs` of the nearest alive cornucopia, or -1 if none.
int nearestCornucopiaIndex(const std::vector<Cornucopia>& cs, sf::Vector2f from) {
    int   bestIdx = -1;
    float minDist = std::numeric_limits<float>::max();
    for (int i = 0; i < (int)cs.size(); i++) {
        if (cs[i].isDestroyed()) continue;
        sf::Vector2f d = cs[i].getPosition() - from;
        float dist = std::sqrt(d.x*d.x + d.y*d.y);
        if (dist < minDist) { minDist = dist; bestIdx = i; }
    }
    return bestIdx;
}

void drawText(sf::RenderWindow& window, sf::Font& font,
              const std::string& str, float x, float y,
              unsigned int size, sf::Color color, bool centered = false)
{
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);

    if (centered) {
        float textWidth = text.getLocalBounds().width;
        x = x - textWidth / 2.f;
    }

    text.setPosition(x, y);
    window.draw(text);
}

int main() {

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Oasis - Prototype");
    window.setFramerateLimit(60);
    sf::Clock clock;

    sf::Font Desert_Road_font;
    Desert_Road_font.loadFromFile("/Users/jamilpalma/Oasis-Repository/assets/fonts/desert_road/Desert_Road.otf");

    HUD hud(Desert_Road_font, WINDOW_WIDTH, WINDOW_HEIGHT);
    int waterPoints = 150;
    int waveNumber  = 1;

    Map map;
    map.loadFromFile("assets/map.txt");
    sf::Vector2i hoveredTile = {-1, -1};

    // Place the starting cornucopia at the center tile
    sf::Vector2i centerTile = map.worldToGrid({ WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f });
    cornucopias.emplace_back(map.tileCenter(centerTile));
    map.setTower(centerTile);

    GameState state = GameState::Menu;

    while (window.isOpen()) {

        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (state == GameState::Menu) {
                if (event.type == sf::Event::KeyPressed &&
                    event.key.code == sf::Keyboard::Enter)
                    state = GameState::Playing;
            }

            if (state == GameState::Won || state == GameState::Lost) {
                if (event.type == sf::Event::KeyPressed &&
                    event.key.code == sf::Keyboard::Enter) {
                    // Reset game
                    towers.clear();
                    enemies.clear();
                    cornucopias.clear();
                    waterPoints = 150;
                    waveNumber  = 1;
                    map = Map();
                    map.loadFromFile("assets/map.txt");
                    centerTile = map.worldToGrid({ WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f });
                    cornucopias.emplace_back(map.tileCenter(centerTile));
                    map.setTower(centerTile);
                    state = GameState::Menu;
                }
            }

            if (event.type == sf::Event::MouseMoved) {
                hoveredTile = map.worldToGrid({ (float)event.mouseMove.x,
                                                (float)event.mouseMove.y });
            }

            if (state == GameState::Playing) {
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                    if (!hud.handleClick(mousePos)) {
                        sf::Vector2i tile = map.worldToGrid(mousePos);

                        if (map.isPlaceable(tile)) {
                            int cost = hud.getSelectedCost();

                            if (waterPoints >= cost) {
                                waterPoints -= cost;
                                map.setTower(tile);

                                if (hud.getSelectedTower() == SelectedTower::Cornucopia) {
                                    cornucopias.emplace_back(map.tileCenter(tile));
                                } else {
                                    towers.push_back(Tower(map.tileCenter(tile)));
                                }
                            }
                        }
                    }
                }
            }
        }

        // ── Update ────────────────────────────────────────────────────────────
        if (state == GameState::Playing) {

            // Count alive cornucopias
            int aliveCornucopias = 0;
            for (const auto& c : cornucopias)
                if (!c.isDestroyed()) aliveCornucopias++;

            // Win: 5 alive cornucopias
            if (aliveCornucopias >= 5) { state = GameState::Won;  goto render; }
            // Lose: all cornucopias gone
            if (aliveCornucopias == 0) { state = GameState::Lost; goto render; }

            hud.update(dt, waterPoints, waveNumber, false, aliveCornucopias);

            if (hud.cycleJustCompleted()) {
                waveNumber++;
                hud.update(dt, waterPoints, waveNumber, true, aliveCornucopias);
            }

            // Cornucopia updates
            for (auto& c : cornucopias) c.update(dt);

            // Enemy spawning and movement
            spawner.update(dt, enemies);

            for (auto& e : enemies) {
                if (!e.isAlive()) continue;

                // Move toward nearest alive cornucopia
                sf::Vector2f target = nearestCornucopia(cornucopias, e.getPosition());
                e.update(dt, target);

                // Damage the nearest cornucopia if the enemy has reached it
                int idx = nearestCornucopiaIndex(cornucopias, e.getPosition());
                if (idx >= 0 && e.hasReachedTarget(cornucopias[idx].getPosition())) {
                    cornucopias[idx].takeDamage(e.getDamage());
                    e.takeDamage(9999.f);           // enemy dies on impact
                    waterPoints += e.getReward();   // earn currency for the kill
                }
            }

            // Remove dead enemies
            enemies.erase(
                std::remove_if(enemies.begin(), enemies.end(),
                               [](const Enemy& e) { return !e.isAlive(); }),
                enemies.end());
        }

        render:
        // ── Render ────────────────────────────────────────────────────────────
        window.clear();

        if (state == GameState::Menu) {
            float cx = WINDOW_WIDTH / 2.f;
            drawText(window, Desert_Road_font, "OASIS",                cx, 200.f, 72, sf::Color::White, true);
            drawText(window, Desert_Road_font, "Press ENTER to start", cx, 340.f, 28, sf::Color(180,180,180), true);
            drawText(window, Desert_Road_font, "Version 0.1",          cx, 500.f, 18, sf::Color(100,100,100), true);
        }

        else if (state == GameState::Playing) {
            map.draw(window, hoveredTile);
            for (auto& c : cornucopias) c.draw(window);
            for (auto& t : towers)      t.draw(window);
            for (auto& e : enemies)     e.draw(window);
            hud.draw(window);
        }

        else if (state == GameState::Won) {
            float cx = WINDOW_WIDTH / 2.f;
            drawText(window, Desert_Road_font, "YOU WIN!",                    cx, 180.f, 72, sf::Color(255,215,0), true);
            drawText(window, Desert_Road_font, "The Oasis is restored.",      cx, 290.f, 32, sf::Color::White,     true);
            drawText(window, Desert_Road_font, "Press ENTER to play again",   cx, 400.f, 24, sf::Color(180,180,180), true);
        }

        else if (state == GameState::Lost) {
            float cx = WINDOW_WIDTH / 2.f;
            drawText(window, Desert_Road_font, "GAME OVER",                   cx, 180.f, 72, sf::Color(200,50,50), true);
            drawText(window, Desert_Road_font, "The last Oasis has fallen.",  cx, 290.f, 32, sf::Color::White,     true);
            drawText(window, Desert_Road_font, "Press ENTER to try again",    cx, 400.f, 24, sf::Color(180,180,180), true);
        }

        window.display();
    }

    return 0;
}
