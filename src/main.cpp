#include <SFML/Graphics.hpp>
#include "Game.h"

int main() {
    const int WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 720;
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Oasis");
    window.setFramerateLimit(60);
    sf::Clock clock;

    Game game(WINDOW_WIDTH, WINDOW_HEIGHT);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)){
            game.processEvent(event, window);
        }

        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        game.update(dt);

        window.clear();
        game.render(window);
        window.display();
    }

    return 0;
}
