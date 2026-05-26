#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include "Game.h"

static constexpr float VIRTUAL_WIDTH  = 1280.f;
static constexpr float VIRTUAL_HEIGHT = 720.f;

// Build a letterboxed view: the virtual 1280x720 area is centered and scaled
// to fit the window while preserving aspect ratio. Excess space becomes black bars.
static sf::View makeLetterboxView(unsigned winW, unsigned winH) {
    sf::View view(sf::FloatRect(0.f, 0.f, VIRTUAL_WIDTH, VIRTUAL_HEIGHT));
    float windowRatio = (float)winW / (float)winH;
    float viewRatio   = VIRTUAL_WIDTH / VIRTUAL_HEIGHT;
    float sizeX = 1.f, sizeY = 1.f, posX = 0.f, posY = 0.f;
    if (windowRatio < viewRatio) {
        sizeY = windowRatio / viewRatio;
        posY  = (1.f - sizeY) / 2.f;
    } else {
        sizeX = viewRatio / windowRatio;
        posX  = (1.f - sizeX) / 2.f;
    }
    view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));
    return view;
}

int main() {
    std::srand(std::time(nullptr));

    sf::RenderWindow window(
        sf::VideoMode((unsigned)VIRTUAL_WIDTH, (unsigned)VIRTUAL_HEIGHT),
        "Oasis",
        sf::Style::Default);
    window.setFramerateLimit(60);

    sf::View view = makeLetterboxView(window.getSize().x, window.getSize().y);
    window.setView(view);

    sf::Clock clock;
    Game game((int)VIRTUAL_WIDTH, (int)VIRTUAL_HEIGHT);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Resized) {
                view = makeLetterboxView(event.size.width, event.size.height);
                window.setView(view);
            }
            game.processEvent(event, window);
        }

        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        game.update(dt);

        window.clear();
        window.setView(view);
        game.render(window);
        window.display();
    }

    return 0;
}
