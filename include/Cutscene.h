#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Cutscene {
public:
    Cutscene(const sf::Font& font, int windowWidth, int windowHeight);

    void update(float dt);
    void render(sf::RenderWindow& window);
    bool isDone() const { return done; }
    void skip();
    void reset();

private:
    enum class Phase { FadeIn, Hold, FadeOut, Gap };

    static constexpr float FADE_IN  = 1.2f;
    static constexpr float HOLD     = 2.f;
    static constexpr float FADE_OUT = 0.9f;
    static constexpr float GAP      = 0.3f;
    static constexpr float LONG_GAP = 1.f; // gap after the "Life" group

    struct Line {
        std::string  text;
        unsigned int charSize;
        bool         longGapAfter;
        std::string  sfx; // empty = no sound
    };

    void playLineSfx();

    const sf::Font& font;
    int winW, winH;

    std::vector<Line> lines;
    int   idx;
    Phase phase;
    float timer;
    bool  done;
};
