#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "SpriteManager.h"

struct AnimatedSprite {
    std::vector<std::string> frames;
    float      fps        = 8.f;
    float      timer      = 0.f;
    int        frameIndex = 0;
    sf::Sprite sprite;

    bool hasFrames() const { return !frames.empty(); }

    void update(float dt) {
        if (frames.empty()) return;
        timer += dt;
        if (timer >= 1.f / fps) {
            timer -= 1.f / fps;
            frameIndex = (frameIndex + 1) % (int)frames.size();
            sprite.setTexture(SpriteManager::get().getTexture(frames[frameIndex]), true);
        }
    }

    void draw(sf::RenderWindow& window, sf::Vector2f pos) {
        if (frames.empty()) return;
        sprite.setPosition(pos);
        window.draw(sprite);
    }
};
