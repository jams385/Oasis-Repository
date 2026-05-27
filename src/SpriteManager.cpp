#include "SpriteManager.h"
#include <stdexcept>

SpriteManager& SpriteManager::get() {
    static SpriteManager instance;
    return instance;
}

void SpriteManager::loadTexture(const std::string& name, const std::string& path) {
    sf::Texture tex;
    if (!tex.loadFromFile(path))
        throw std::runtime_error("SpriteManager: failed to load " + path);
    textures[name] = std::move(tex);
}

sf::Texture& SpriteManager::getTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it == textures.end())
        throw std::runtime_error("SpriteManager: texture not found: " + name);
    return it->second;
}
