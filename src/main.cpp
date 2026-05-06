#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "HUD.h"
#include "Map.h"
#include "Tower.h"

std::vector<Tower> towers;

using namespace std;

/* read program documentation for explanations:
https://docs.google.com/document/d/1sXYtkZC9QKFjfhb1YHDYnqGQbtqAIGhFicqRt7q9XC8/edit?tab=t.eynqtmny1yz4 */

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

enum class GameState{
    Menu,
    Playing
};

/* helper function for making texts */
void drawText(sf::RenderWindow& window, sf::Font& font,
              const std::string& str, float x, float y,
              unsigned int size, sf::Color color, bool centered = false)
{
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);

    if (centered) {
        float textWidth = text.getLocalBounds().width;
        x = x - textWidth / 2.f;  // shift left by half the text width
    }

    text.setPosition(x, y);
    window.draw(text);
}

int main() {

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Oasis - Prototype");
    window.setFramerateLimit(60);
    sf::Clock clock;

    /* gets font*/
    sf::Font Desert_Road_font;
    Desert_Road_font.loadFromFile("/Users/jamilpalma/Oasis-Repository/assets/fonts/desert_road/Desert_Road.otf");

    HUD hud(Desert_Road_font, WINDOW_WIDTH, WINDOW_HEIGHT);
    int waterPoints = 150;
    int waveNumber = 1;

    Map map;
    map.loadFromFile("assets/map.txt");
    sf::Vector2i hoveredTile = {-1, -1};


    GameState state = GameState::Menu;

    /* GAME LOOP */
    while (window.isOpen()) {

        float DeltaTime = clock.restart().asSeconds();
        if (DeltaTime > 0.1f) DeltaTime = 0.1f;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed){
                window.close();
            }

            if (state == GameState::Menu){
                if(event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Enter){
                    state = GameState::Playing;
                }
            }

            if (state == GameState::Playing) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    hud.handleClick(mousePos);
                }
            }

            if(event.type == sf::Event::MouseMoved){
                hoveredTile = map.worldToGrid({(float)event.mouseMove.x, (float)event.mouseMove.y});
            }

            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                if(!hud.handleClick(mousePos)){
                    sf::Vector2i tile = map.worldToGrid(mousePos);

                    if(map.isPlaceable(tile)){
                        map.setTower(tile);
                        towers.push_back(Tower(map.tileCenter(tile)));
                    }
                }
            }

        }

        // UPDATES HUD
        if (state == GameState::Playing) {
            hud.update(DeltaTime, waterPoints, waveNumber, false);

            if (hud.cycleJustCompleted()) {
                waveNumber++;
                hud.update(DeltaTime, waterPoints, waveNumber, true);
            }
        }
        
        // RENDER
        window.clear();

        if(state == GameState::Menu){

            float centerX = WINDOW_WIDTH / 2.F;
            drawText(window, Desert_Road_font, "OASIS",                centerX, 200.f, 72, sf::Color::White, true);
            drawText(window, Desert_Road_font, "Press ENTER to start", centerX, 340.f, 28, sf::Color(180,180,180), true);
            drawText(window, Desert_Road_font, "Version 0.1",          centerX, 500.f, 18, sf::Color(100,100,100), true);
        }

        else if(state == GameState::Playing){
            map.draw(window, hoveredTile);
            for (auto& t : towers) t.draw(window);
            hud.draw(window);
        }
 
        window.display();
    }

    return 0;
}