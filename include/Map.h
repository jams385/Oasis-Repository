#pragma once
#include <SFML/Graphics.hpp>
#include <string>

constexpr int TILE_EMPTY   = 0;  // placeable ground
constexpr int TILE_BLOCKED = 2;  // decorative, can't build
constexpr int TILE_TOWER   = 3;  // occupied by a tower

constexpr int TILE_SIZE = 32;
constexpr int GRID_COLS = 40; // formula: 1280/tile size
constexpr int GRID_ROWS = 22; // formula: 720/tile size

class Map {
public:
    Map();

    bool loadFromFile(const std::string& filepath);

    void draw(sf::RenderWindow& window, sf::Vector2i hoveredTile);

    bool isPlaceable(sf::Vector2i tile) const;
    void setTower(sf::Vector2i tile);

    sf::Vector2i worldToGrid(sf::Vector2f worldPos) const;
    sf::Vector2f gridToWorld(sf::Vector2i tile)     const;
    sf::Vector2f tileCenter(sf::Vector2i tile)      const;

private:
    int grid[GRID_ROWS][GRID_COLS];
};