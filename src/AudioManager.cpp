#include "AudioManager.h"
#include <stdexcept>

AudioManager::AudioManager() {
    soundPool.reserve(16);
}

AudioManager& AudioManager::get() {
    static AudioManager instance;
    return instance;
}

void AudioManager::loadSound(const std::string& name, const std::string& path) {
    sf::SoundBuffer buf;
    if (!buf.loadFromFile(path))
        throw std::runtime_error("AudioManager: failed to load " + path);
    buffers[name] = std::move(buf);
}

void AudioManager::play(const std::string& name, float volume) {
    auto it = buffers.find(name);
    if (it == buffers.end()) return;

    // clean finished sounds first to keep pool small
    soundPool.erase(
        std::remove_if(soundPool.begin(), soundPool.end(),
            [](const sf::Sound& s){ return s.getStatus() == sf::Sound::Stopped; }),
        soundPool.end()
    );

    soundPool.emplace_back(it->second);
    soundPool.back().setVolume(volume);
    soundPool.back().play();
}

void AudioManager::playMusic(const std::string& path, bool loop) {
    music.stop();
    if (!music.openFromFile(path)) return;
    music.setLoop(loop);
    music.play();
}

void AudioManager::stopMusic() {
    music.stop();
}

void AudioManager::setMusicVolume(float volume) {
    music.setVolume(volume);
}

void AudioManager::stopAllSounds() {
    for (auto& s : soundPool) s.stop();
    soundPool.clear();
}

void AudioManager::update() {
    soundPool.erase(
        std::remove_if(soundPool.begin(), soundPool.end(),
            [](const sf::Sound& s){ return s.getStatus() == sf::Sound::Stopped; }),
        soundPool.end()
    );
}
