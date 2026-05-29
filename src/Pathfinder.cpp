#include "Pathfinder.h"
#include <queue>
#include <algorithm>
#include <cmath>

namespace {

    struct Node {
        sf::Vector2i pos;
        float g, f;
    };

    struct NodeCmp {
        bool operator()(const Node& a, const Node& b) const {
            return a.f > b.f;
        }
    };

    /* Estimates remaining distance to goal using octile distance */
    inline float heuristic(sf::Vector2i a, sf::Vector2i b) {
        float dx = std::abs((float)(a.x - b.x));
        float dy = std::abs((float)(a.y - b.y));
        return std::max(dx, dy) + (std::sqrt(2.f) - 1.f) * std::min(dx, dy);
    }

}

std::vector<sf::Vector2f> Pathfinder::findPath(const Map& map, sf::Vector2f startWorld, sf::Vector2f goalWorld) {

    sf::Vector2i start = map.worldToGrid(startWorld);
    sf::Vector2i goal  = map.worldToGrid(goalWorld);

    /* Clamp so we don't access outside the grid */
    if (start.x < 0) start.x = 0; if (start.x >= GRID_COLS) start.x = GRID_COLS - 1;
    if (start.y < 0) start.y = 0; if (start.y >= GRID_ROWS) start.y = GRID_ROWS - 1;
    if (goal.x  < 0) goal.x  = 0; if (goal.x  >= GRID_COLS) goal.x  = GRID_COLS - 1;
    if (goal.y  < 0) goal.y  = 0; if (goal.y  >= GRID_ROWS) goal.y  = GRID_ROWS - 1;

    if (start == goal) return { goalWorld };

    /* 8 directions: right, left, down, up, and the 4 diagonals */
    constexpr int   DX[8]   = {  1, -1,  0,  0,  1, -1,  1, -1 };
    constexpr int   DY[8]   = {  0,  0,  1, -1,  1,  1, -1, -1 };
    constexpr float COST[8] = { 1.f, 1.f, 1.f, 1.f, 1.41421f, 1.41421f, 1.41421f, 1.41421f };

    /* Initialize g scores to a large number and visited to false */
    float        gScore[GRID_ROWS][GRID_COLS];
    bool         visited[GRID_ROWS][GRID_COLS];
    sf::Vector2i parent[GRID_ROWS][GRID_COLS];

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            gScore[r][c]  = 999999.f;
            visited[r][c] = false;
            parent[r][c]  = { -1, -1 };
        }
    }
    
    /* initialize a priority_queue node */
    std::priority_queue<Node, std::vector<Node>, NodeCmp> open;

    /* push heuristic() */
    gScore[start.y][start.x] = 0.f;
    open.push({ start, 0.f, heuristic(start, goal) });

    while (!open.empty()) {
        Node cur = open.top();
        open.pop();

        /* Skip if already processed */
        if (visited[cur.pos.y][cur.pos.x]) continue;
        visited[cur.pos.y][cur.pos.x] = true;

        if (cur.pos == goal) {
            /* Reconstruct path by walking parent pointers from goal back to start */
            std::vector<sf::Vector2f> path;
            sf::Vector2i p = goal;
            while (!(p == start)) {
                path.push_back(map.tileCenter(p));
                p = parent[p.y][p.x];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (int i = 0; i < 8; i++) {
            sf::Vector2i next { cur.pos.x + DX[i], cur.pos.y + DY[i] };

            if (next.x < 0 || next.x >= GRID_COLS || next.y < 0 || next.y >= GRID_ROWS)
                continue;

            if (visited[next.y][next.x]) continue;

            /* Allow goal tile even if it has a tower/cornucopia on it */
            bool walkable = map.isPlaceable(next) || next == goal;

            /* Diagonal Walk Checks for adjacent tiles, making sure it cannot "squeeze" through structures */
            if (i >= 4) {
                if (!map.isPlaceable({ cur.pos.x + DX[i], cur.pos.y })) continue;
                if (!map.isPlaceable({ cur.pos.x, cur.pos.y + DY[i] })) continue;
            }

            if (!walkable) continue;

            float newG = cur.g + COST[i];

            if (newG < gScore[next.y][next.x]) {
                gScore[next.y][next.x] = newG;
                parent[next.y][next.x] = cur.pos;
                float f = newG + heuristic(next, goal);
                open.push({ next, newG, f });
            }
        }
    }

    /* No path found - fall back to direct movement */
    return { goalWorld };
}
