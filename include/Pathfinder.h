#pragma once
#include <vector>
#include "Map.h"

class Pathfinder{
public:
    static std::vector<sf::Vector2f> findPath(
        const Map& map,
        sf::Vector2f startWorld,
        sf::Vector2f goalWorld
    );
};


