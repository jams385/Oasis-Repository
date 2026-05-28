#include "Map.h"
#include <fstream>
#include <iostream>

const sf::Color COLOR_EMPTY   = sf::Color( 0, 0, 0, 0);
const sf::Color COLOR_BLOCKED = sf::Color( 40,  60,  40);
const sf::Color COLOR_TOWER   = sf::Color( 50,  80,  60);
const sf::Color COLOR_HOVER   = sf::Color(110, 160,  90);
const sf::Color COLOR_GRID    = sf::Color(  0,   0,   0,  40);

Map::Map() {
    for (int r = 0; r < GRID_ROWS; r++)
        for (int c = 0; c < GRID_COLS; c++)
            grid[r][c] = TILE_EMPTY;
}

bool Map::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Map: could not open " << filepath << "\n";
        return false;
    }

    int row = 0;
    std::string line;
    while (std::getline(file, line) && row < GRID_ROWS) {
        for (int col = 0; col < GRID_COLS && col < (int)line.size(); col++) {
            char c = line[col];
            if      (c == '0') grid[row][col] = TILE_EMPTY;
            else if (c == '2') grid[row][col] = TILE_BLOCKED;
            else                grid[row][col] = TILE_EMPTY;
        }
        row++;
    }

    return true;
}

void Map::draw(sf::RenderWindow& window, sf::Vector2i hoveredTile) {
    sf::RectangleShape tile({(float)TILE_SIZE - 1, (float)TILE_SIZE - 1});
    tile.setOutlineColor(COLOR_GRID);
    tile.setOutlineThickness(0.f);

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            tile.setPosition(c * TILE_SIZE, r * TILE_SIZE);

            switch (grid[r][c]) {
                case TILE_BLOCKED: tile.setFillColor(COLOR_BLOCKED); break;
                case TILE_TOWER:   tile.setFillColor(COLOR_TOWER);   break;
                default:           tile.setFillColor(COLOR_EMPTY);   break;
            }

            if (c == hoveredTile.x && r == hoveredTile.y
                && grid[r][c] == TILE_EMPTY)
                tile.setFillColor(COLOR_HOVER);

            window.draw(tile);
        }
    }
}

bool Map::isPlaceable(sf::Vector2i tile) const {
    if (tile.x < 0 || tile.x >= GRID_COLS) return false;
    if (tile.y < 0 || tile.y >= GRID_ROWS)  return false;
    return grid[tile.y][tile.x] == TILE_EMPTY;
}

void Map::setTower(sf::Vector2i tile) {
    if (tile.x < 0 || tile.x >= GRID_COLS) return;
    if (tile.y < 0 || tile.y >= GRID_ROWS)  return;
    grid[tile.y][tile.x] = TILE_TOWER;
}

void Map::clearTower(sf::Vector2i tile) {
    if (tile.x < 0 || tile.x >= GRID_COLS) return;
    if (tile.y < 0 || tile.y >= GRID_ROWS)  return;
    if (grid[tile.y][tile.x] == TILE_TOWER)
        grid[tile.y][tile.x] = TILE_EMPTY;
}

sf::Vector2i Map::worldToGrid(sf::Vector2f worldPos) const {
    return {
        (int)(worldPos.x / TILE_SIZE),
        (int)(worldPos.y / TILE_SIZE)
    };
    
}

sf::Vector2f Map::gridToWorld(sf::Vector2i tile) const {
    return {
        (float)(tile.x * TILE_SIZE),
        (float)(tile.y * TILE_SIZE)
    };
}

sf::Vector2f Map::tileCenter(sf::Vector2i tile) const {
    return {
        tile.x * TILE_SIZE + TILE_SIZE / 2.f,
        tile.y * TILE_SIZE + TILE_SIZE / 2.f
    };
}