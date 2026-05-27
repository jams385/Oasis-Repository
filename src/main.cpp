#include <cstdlib>
#include <ctime>
#include "Window.h"
#include "Game.h"

int main() {
    std::srand(std::time(nullptr));

    Window window(1280u, 720u, "Oasis");
    sf::Clock clock;
    Game game(1280, 720);

    while (window.isOpen()) {
        sf::Event event;
        while (static_cast<sf::RenderWindow&>(window).pollEvent(event)) {
            window.handleEvent(event);    // zoom, pan, resize, camera reset
            game.processEvent(event, window);
        }

        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        window.update(dt); // keyboard camera pan
        game.update(dt);

        window.clear();
        game.render(window);
        window.display();
    }

    return 0;
}
