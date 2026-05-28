#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Map.h"

class Window {
public:
    static constexpr float VIRTUAL_W = 1280.f;
    static constexpr float VIRTUAL_H = 720.f;
    static constexpr float MAP_W     = TILE_SIZE * GRID_COLS * 1.f;
    static constexpr float MAP_H     = TILE_SIZE * GRID_ROWS * 1.f;

    static constexpr float ZOOM_MIN  = 0.75f;
    static constexpr float ZOOM_MAX  = 4.f;
    static constexpr float PAN_SPEED = 500.f; // screen-space pixels/sec

    sf::View         _worldView;

    Window(unsigned width, unsigned height, const std::string& title);

    // Pass-throughs so Game doesn't need to reach into _win directly
    bool isOpen()  const { return _win.isOpen(); }
    void display()       { _win.display(); }
    void close()         { _win.close(); }
    void draw(const sf::Drawable& d) { _win.draw(d); }
    void clear(const sf::Color& color = sf::Color::Black){
        _win.clear(color);}

    // Implicit conversion lets Map/HUD/Tower/etc. take sf::RenderWindow& directly
    operator sf::RenderWindow&() { return _win; }

    // Process one SFML event (zoom, pan, resize, camera reset)
    void handleEvent(const sf::Event& event);

    // Call every frame so held arrow/WASD keys pan the camera
    void update(float dt);

    // Apply the correct view before drawing world objects or HUD
    void setWorldView();
    void setHUDView();

    // Scaling and Align Background Sprites for Window
    void scaleAndCenterSprite(sf::Sprite& sprite) const;

    // Map a raw pixel to world or HUD (virtual 1280x720) coordinates
    sf::Vector2f mapPixelToCoords(sf::Vector2i pixel) const; // → world view
    sf::Vector2f mapMouseToHUD(sf::Vector2i pixel)    const; // → HUD view

private:
    sf::RenderWindow _win;
    sf::View         _hudView;

    float        _zoom      = 1.f;
    bool         _panActive = false;
    sf::Vector2i _lastMouse;

    sf::View makeLetterbox(unsigned winW, unsigned winH) const;
    void     clampWorldView();
};
