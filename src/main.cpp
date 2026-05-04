#include <SFML/Graphics.hpp>
#include "Enemy.h"
#include "Constants.h"
#include "Tower.h"

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Oasis - Prototype");
    window.setFramerateLimit(60);
    sf::Clock clock;

    std::vector<Enemy> enemies;
    enemies.push_back(Enemy({ 0.f, 300.f }, 120.f, 100.f));

    Tower tower({ WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f }, 200.f, 1.f, 20.f);

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        for (auto& e : enemies) e.update(dt);
        tower.update(dt, enemies);

        window.clear(sf::Color(30, 40, 25));
        for (auto& e : enemies) e.draw(window);
        tower.draw(window);
        window.display();
    }

    return 0;
}