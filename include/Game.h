#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Map.h"
#include "Tower.h"
#include "Enemy.h"
#include "EnemySpawner.h"
#include "Cornucopia.h"
#include "Bullet.h"
#include "HUD.h"
#include "WaterMine.h"

enum class GameState { Menu, Playing, Won, Lost };

class Game {
public:

    Game(int windowWidth, int windowHeight);

    void processEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(float dt);
    void render(sf::RenderWindow& window);

private:

    int       windowWidth;
    int       windowHeight;
    GameState state;

    sf::Font     font;
    Map          map;
    EnemySpawner spawner;
    HUD          hud;

    std::vector<Tower>      towers;
    std::vector<Enemy>      enemies;
    std::vector<Cornucopia> cornucopias;
    std::vector<Bullet>     bullets;
    std::vector<WaterMine>  waterMines;

    int          waterPoints;
    int          waveNumber;
    sf::Vector2i hoveredTile;

    void reset();
    void handlePlacement(sf::Vector2i tile);

    sf::Vector2f nearestCornucopiaPos(sf::Vector2f from) const;
    int          nearestCornucopiaIdx(sf::Vector2f from) const;

    static void drawText(sf::RenderWindow& window, sf::Font& font,
                         const std::string& str, float x, float y,
                         unsigned int size, sf::Color color, bool centered = false);
};
