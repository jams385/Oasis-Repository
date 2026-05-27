#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class AudioManager {
public:
    static AudioManager& get();

    void loadSound(const std::string& name, const std::string& path);
    void play(const std::string& name, float volume = 100.f);

    void playMusic(const std::string& path, bool loop = true);
    void stopMusic();
    void setMusicVolume(float volume);

    void update(); // call each frame to clean up finished sounds
    void stopAllSounds();

private:
    AudioManager();

    std::unordered_map<std::string, sf::SoundBuffer> buffers;
    std::vector<sf::Sound>                           soundPool;
    sf::Music                                        music;
};
