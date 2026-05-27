#include "Cutscene.h"

Cutscene::Cutscene(const sf::Font& font, int windowWidth, int windowHeight)
    : font(font)
    , winW(windowWidth)
    , winH(windowHeight)
    , idx(0)
    , phase(Phase::FadeIn)
    , timer(0.f)
    , done(false)
{
    lines = {
        { "Fields",                   42, false },
        { "Forests",                  42, false },
        { "Oceans",                   42, false },
        { "Life",                     42, true  },
        { "We have to fix our pasts mistakes", 28, false },
        { "Restore our fallen cornucopias",    28, false },
        { "Build our Oasis",          32, false },
    };
}

void Cutscene::reset() {
    idx   = 0;
    phase = Phase::FadeIn;
    timer = 0.f;
    done  = false;
}

void Cutscene::skip() {
    done = true;
}

void Cutscene::update(float dt) {
    if (done) return;
    timer += dt;

    float gapDur = (idx < (int)lines.size() && lines[idx].longGapAfter) ? LONG_GAP : GAP;

    switch (phase) {
    case Phase::FadeIn:
        if (timer >= FADE_IN)  { timer -= FADE_IN;  phase = Phase::Hold;    } break;
    case Phase::Hold:
        if (timer >= HOLD)     { timer -= HOLD;     phase = Phase::FadeOut; } break;
    case Phase::FadeOut:
        if (timer >= FADE_OUT) { timer -= FADE_OUT; phase = Phase::Gap;     } break;
    case Phase::Gap:
        if (timer >= gapDur) {
            timer -= gapDur;
            idx++;
            if (idx >= (int)lines.size()) done = true;
            else phase = Phase::FadeIn;
        }
        break;
    }
}

void Cutscene::render(sf::RenderWindow& window) {
    sf::RectangleShape bg(sf::Vector2f((float)winW, (float)winH));
    bg.setFillColor(sf::Color::Black);
    window.draw(bg);

    if (done || idx >= (int)lines.size()) return;

    float alpha = 0.f;
    switch (phase) {
    case Phase::FadeIn:  alpha = (timer / FADE_IN)          * 255.f; break;
    case Phase::Hold:    alpha = 255.f;                               break;
    case Phase::FadeOut: alpha = (1.f - timer / FADE_OUT)   * 255.f; break;
    case Phase::Gap:     alpha = 0.f;                                 break;
    }
    if (alpha < 0.f)   alpha = 0.f;
    if (alpha > 255.f) alpha = 255.f;

    const Line& line = lines[idx];
    sf::Text text;
    text.setFont(font);
    text.setString(line.text);
    text.setCharacterSize(line.charSize);
    text.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));

    sf::FloatRect b = text.getLocalBounds();
    text.setPosition(
        winW / 2.f - b.left - b.width  / 2.f,
        winH / 2.f - b.top  - b.height / 2.f
    );
    window.draw(text);
}
