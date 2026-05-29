#include "Pathfinder.h"
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace {

    struct Node{
        sf::Vector2i pos;
        float g, f;
    };

    struct NodeCmp{
        bool operator() (const Node& a, const Node& b) const {
            return a.f > b.f;
        }
    };

    /* Converts the 2D GRID into a 1D ARRAY index */
    inline int encode(sf::Vector2i p){
        return p.y * GRID_COLS + p.x;
    }

    /*  Calculates distance between diagonal tiles.
        heuristic = full path still needed to walk */
    inline float heuristic(sf::Vector2i a, sf::Vector2i b){

        float dx = std::abs((float(a.x - b.x)));
        float dy = std::abs((float)(a.y - b.y));

        /* 
            max(dx, dy):                counts all of the moves 
            (√2 - 1) * min(dx, dy):     extra cost for diagonals

            max(dx, dy) + ((√2 - 1) * min(dx, dy)): counts all the moves then adds to account for extra cost of diagonals
        */
        return std::max(dx, dy) + (std::sqrt(2.f) - 1.f) * std::min(dx, dy);
    }

}

std::vector<sf::Vector2f> Pathfinder::findPath(const Map& map, sf::Vector2f startWorld, sf::Vector2f goalWorld){
    
    sf::Vector2i start = map.worldToGrid(startWorld);
    sf::Vector2i goal = map.worldToGrid(goalWorld);

    /* Lambda - anonymous functions */
    auto clamp = [](int v, int lo, int hi){

        if (v < lo) return lo;

        if (v > hi) return hi;

        return v;
    };

    /* We clamp the starting and end position so that we don't access indices outside the grid */
    start.x = clamp(start.x, 0, GRID_COLS - 1);
    start.y = clamp(start.y, 0, GRID_ROWS - 1);
    goal.x = clamp(goal.x, 0, GRID_COLS - 1);
    goal.y = clamp(goal.y, 0, GRID_ROWS - 1);

    /* if enemy is in it's target tile, return*/
    if (start == goal){
        return { goalWorld };
    }

    /* DEFINES 8 MULTI-DIRECTIONAL MOVEMENT and its corresponding cost*/
    constexpr int DX[8] =       { 1,    -1,  0,   0,    1,          -1,      1,         -1          };
    constexpr int DY[8] =       { 0,    0,   1,  -1,    1,          1,      -1,         -1          };
    constexpr float COST[8] =   { 1.f,  1.f, 1.f, 1.f,  1.41421f,   1.41421f,1.41421f,   1.41421f   };

    /* 
        priority queue is part of <queue> -> A data struct that gives you the smalles/largest element first when you pop.
        unordered_map is part of <unordered_map> -> It is a hash map.
    */

    std::priority_queue<Node, std::vector<Node>, NodeCmp> open;
    std::unordered_map<int, float>        gScore;
    std::unordered_map<int, sf::Vector2i> parent;    

    /*  FLOW

        gScore -> encodes starting position (1D array index) -> assign a score of 0.f.

        open -> pushes: pos = starting tile,
                        g = 0.f (distance traveled so far)
                        f = heuristic(start, goal) = estimated total path length

        loop while open has elements -> checks in order of priority -> pop after storing info
        -> if the current position is the same as the goal, push the tiles from goal to start
    */
    gScore[encode(start)] = 0.f;
    open.push({start, 0.f, heuristic(start, goal)});

    while (!open.empty()) {
        Node cur = open.top(); // peek at priority node
        open.pop(); // remove

        if (cur.pos == goal) {

            std::vector<sf::Vector2f> path;
            sf::Vector2i p = goal;

            while (!(p == start)) {
                path.push_back(map.tileCenter(p));
                p = parent[encode(p)];
            }

        std::reverse(path.begin(), path.end());
        return path;
        }

        int curKey = encode(cur.pos);

        /* 
            .count() -> 1 if key exists, 0 if doesn't
            .at() -> returns the value of the key

            "if the tile already has a gScore and it's worse then the path we found, skip it"
        */
        if(gScore.count(curKey) && cur.g > gScore.at(curKey) + 1e-4f){
            continue;
        }

        for (int i=0; i < 8; i++){
            sf::Vector2i next { cur.pos.x + DX[i], cur.pos.y + DY[i] };

            if (next.x < 0 || next.x >= GRID_COLS || next.y < 0 || next.y >= GRID_ROWS) 
                continue;
            
            bool walkable = map.isPlaceable(next) || next == goal;

            /* Diagonal Walk Checks for adjacent tiles, making sure it cannot "squeeze" through structures */
            if (i >= 4) {
                if (!map.isPlaceable({ cur.pos.x + DX[i], cur.pos.y })) continue;
                if (!map.isPlaceable({ cur.pos.x, cur.pos.y + DY[i] })) continue;
            }

            if(!walkable) continue;

            float newG = cur.g + COST[i];
            int nKey = encode(next);

            if(!gScore.count(nKey) || newG < gScore.at(nKey)){
                gScore[nKey] = newG;
                parent[nKey] = cur.pos;
                open.push({next, newG, newG + heuristic(next, goal)});
            }
        }      
    }
    
    return { goalWorld };
}