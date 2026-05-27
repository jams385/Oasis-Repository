#include "Window.h"
#include <algorithm>

Window::Window(unsigned width, unsigned height, const std::string& title)
    : _win(sf::VideoMode(width, height), title, sf::Style::Default)
{
    _win.setFramerateLimit(60);

    _hudView   = makeLetterbox(width, height);
    _worldView = _hudView;
    _worldView.setCenter(MAP_W / 2.f, MAP_H / 2.f);
    _worldView.setSize(VIRTUAL_W / _zoom, VIRTUAL_H / _zoom);
}

// ── View helpers ──────────────────────────────────────────────────────────────

sf::View Window::makeLetterbox(unsigned winW, unsigned winH) const {
    sf::View view(sf::FloatRect(0.f, 0.f, VIRTUAL_W, VIRTUAL_H));
    float windowRatio = (float)winW / (float)winH;
    float viewRatio   = VIRTUAL_W / VIRTUAL_H;
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

void Window::clampWorldView() {
    float halfW = (VIRTUAL_W / _zoom) / 2.f;
    float halfH = (VIRTUAL_H / _zoom) / 2.f;

    float minX = halfW,        maxX = MAP_W - halfW;
    float minY = halfH,        maxY = MAP_H - halfH;

    // If the visible area is wider/taller than the map, just center
    if (minX > maxX) { minX = maxX = MAP_W / 2.f; }
    if (minY > maxY) { minY = maxY = MAP_H / 2.f; }

    sf::Vector2f c = _worldView.getCenter();
    c.x = std::clamp(c.x, minX, maxX);
    c.y = std::clamp(c.y, minY, maxY);
    _worldView.setCenter(c);
}

// ── Event handling ────────────────────────────────────────────────────────────

void Window::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::Resized) {
        _hudView = makeLetterbox(event.size.width, event.size.height);
        _worldView.setViewport(_hudView.getViewport());
    }

    if (event.type == sf::Event::MouseWheelScrolled &&
        event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
    {
        float delta = event.mouseWheelScroll.delta;
        if (std::abs(delta) < 0.01f) return; // ignore noise

        // On some macOS setups the sign is inverted; treat positive as zoom-in
        // and negative as zoom-out regardless of magnitude
        float factor = (delta > 0.f) ? 1.2f : (1.f / 1.2f);

        sf::Vector2i pixel = { (int)event.mouseWheelScroll.x,
                               (int)event.mouseWheelScroll.y };
        sf::Vector2f before = _win.mapPixelToCoords(pixel, _worldView);

        _zoom = std::clamp(_zoom * factor, ZOOM_MIN, ZOOM_MAX);
        _worldView.setSize(VIRTUAL_W / _zoom, VIRTUAL_H / _zoom);

        sf::Vector2f after = _win.mapPixelToCoords(pixel, _worldView);
        _worldView.move(before - after); // keep world point under cursor fixed
        clampWorldView();
    }

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Middle)
    {
        _panActive = true;
        _lastMouse = { event.mouseButton.x, event.mouseButton.y };
    }
    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Middle)
    {
        _panActive = false;
    }
    if (event.type == sf::Event::MouseMoved && _panActive) {
        sf::Vector2i curr = { event.mouseMove.x, event.mouseMove.y };
        sf::Vector2f prev = _win.mapPixelToCoords(_lastMouse, _worldView);
        sf::Vector2f now  = _win.mapPixelToCoords(curr,       _worldView);
        _worldView.move(prev - now);
        clampWorldView();
        _lastMouse = curr;
    }

    // R resets the camera to the default (full-map) view
    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::R)
    {
        _zoom = 1.f;
        _worldView.setSize(VIRTUAL_W, VIRTUAL_H);
        _worldView.setCenter(MAP_W / 2.f, MAP_H / 2.f);
        clampWorldView();
    }
}

// ── Per-frame keyboard pan ────────────────────────────────────────────────────

void Window::update(float dt) {
    float dx = 0.f, dy = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)  ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::A))      dx -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D))      dx += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)    ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::W))      dy -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)  ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::S))      dy += 1.f;

    if (dx != 0.f || dy != 0.f) {
        if (dx != 0.f && dy != 0.f) { dx *= 0.7071f; dy *= 0.7071f; }
        // PAN_SPEED is in screen-space px/s: dividing by _zoom gives consistent
        // on-screen speed regardless of zoom level
        _worldView.move(dx * PAN_SPEED / _zoom * dt,
                        dy * PAN_SPEED / _zoom * dt);
        clampWorldView();
    }
}

// ── View activation ───────────────────────────────────────────────────────────

void Window::setWorldView() { _win.setView(_worldView); }
void Window::setHUDView()   { _win.setView(_hudView);   }

// ── Coordinate mapping ────────────────────────────────────────────────────────

sf::Vector2f Window::mapPixelToCoords(sf::Vector2i pixel) const {
    return _win.mapPixelToCoords(pixel, _worldView);
}

sf::Vector2f Window::mapMouseToHUD(sf::Vector2i pixel) const {
    return _win.mapPixelToCoords(pixel, _hudView);
}
