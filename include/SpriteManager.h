#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

class SpriteManager {
public:
    static SpriteManager& get();

    void         loadTexture(const std::string& name, const std::string& path);
    sf::Texture& getTexture(const std::string& name);

private:
    SpriteManager() = default;
    std::unordered_map<std::string, sf::Texture> textures;
};
