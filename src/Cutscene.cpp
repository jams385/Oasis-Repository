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

    backgrounds.resize(7);

    backgrounds[0].loadFromFile("assets/OASIS-GRAPHICS/MENU-CUTSCENE_GRAPHICS/3.png");
    backgrounds[1].loadFromFile("assets/OASIS-GRAPHICS/MENU-CUTSCENE_GRAPHICS/4.png");
    backgrounds[2].loadFromFile("assets/OASIS-GRAPHICS/MENU-CUTSCENE_GRAPHICS/5.png");
    backgrounds[3].loadFromFile("assets/OASIS-GRAPHICS/MENU-CUTSCENE_GRAPHICS/6.png");
    backgrounds[4].loadFromFile("assets/OASIS-GRAPHICS/MENU-CUTSCENE_GRAPHICS/7.png");
    backgrounds[5].loadFromFile("assets/OASIS-GRAPHICS/MENU-CUTSCENE_GRAPHICS/8.png");
    backgrounds[6].loadFromFile("assets/OASIS-GRAPHICS/MENU-CUTSCENE_GRAPHICS/9.png");
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

    float gapDur  = GAP;
    float holdDur = HOLD;

    switch (phase) {
    case Phase::FadeIn:
        if (timer >= FADE_IN)  { timer -= FADE_IN;  phase = Phase::Hold;    } break;
    case Phase::Hold:
        if (timer >= HOLD)  { timer -= HOLD;  phase = Phase::FadeOut; } break;
    case Phase::FadeOut:
        if (timer >= FADE_OUT) { timer -= FADE_OUT; phase = Phase::Gap;     } break;
    case Phase::Gap:
        if (timer >= gapDur) {
            timer -= gapDur;
            idx++;
            if (idx >= (int)backgrounds.size()) done = true;
            // if (idx >= (int)backgrounds.size()) done = true;
            else phase = Phase::FadeIn;
        }
        break;
    }
}

void Cutscene::render(sf::RenderWindow& window) {
    float alpha = 0.f;
        switch (phase) {
        case Phase::FadeIn:  alpha = (timer / FADE_IN)          * 255.f; break;
        case Phase::Hold:    alpha = 255.f;                               break;
        case Phase::FadeOut: alpha = (1.f - timer / FADE_OUT)   * 255.f; break;
        case Phase::Gap:     alpha = 0.f;                                 break;
    }

    backgroundSprite.setTexture(backgrounds[idx]);

    //scaling
        sf::Vector2u texSize = backgrounds[idx].getSize();
        float scale = std::max(
            (float)winW / texSize.x,
            (float)winH / texSize.y
        );

        backgroundSprite.setColor(
        sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha))
        );

        backgroundSprite.setScale(scale, scale);
        backgroundSprite.setPosition(
            (winW - texSize.x * scale) / 2.f,
            (winH - texSize.y * scale) / 2.f
        );

    window.draw(backgroundSprite);
    if (done || idx >= (int)backgrounds.size()) return;

    if (alpha < 0.f)   alpha = 0.f;
    if (alpha > 255.f) alpha = 255.f;

    

    // const Line& line = lines[idx];
    // sf::Text text;
    // text.setFont(font);
    // text.setString(line.text);
    // text.setCharacterSize(line.charSize);
    // text.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));

    // sf::FloatRect b = text.getLocalBounds();
    // text.setPosition(
    //     winW / 2.f - b.left - b.width  / 2.f,
    //     winH / 2.f - b.top  - b.height / 2.f
    // );
    // window.draw(text);
}
