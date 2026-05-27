#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Map.h"
#include "Window.h"
#include "Tower.h"
#include "Enemy.h"
#include "EnemySpawner.h"
#include "Cornucopia.h"
#include "Bullet.h"
#include "HUD.h"
#include "WaterMine.h"

enum class GameState { Menu, Playing, Paused, Won, Lost };

class Game {
public:

    Game(int windowWidth, int windowHeight);

    void processEvent(const sf::Event& event, Window& window);
    void update(float dt);
    void render(Window& window);

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

    bool          wasNight       = false;
    int           sellTowerIdx   = -1;
    int           sellMineIdx    = -1;
    sf::FloatRect sellPopupRect;
    int           selectedCornIdx = -1;

    void reset();
    void placeCornucopias();
    void handlePlacement(sf::Vector2i tile);
    bool isAdjacentToCornucopia(sf::Vector2i tile) const;

    struct NearestTarget { int cornIdx = -1, towIdx = -1, wmIdx = -1; };
    NearestTarget findNearestTarget(sf::Vector2f from) const;

    sf::Vector2f nearestTargetPos(sf::Vector2f from) const;
    void         damageNearestTarget(sf::Vector2f from, float damage);

    static void drawText(sf::RenderWindow& window, sf::Font& font,
                         const std::string& str, float x, float y,
                         unsigned int size, sf::Color color, bool centered = false);
};
