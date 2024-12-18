#include "Player.hpp"

#include <format>

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/rotate_vector.hpp>

#include "game/Game.hpp"
#include "engine/Renderer.hpp"
#include "engine/Gradient.h"

constexpr int TICKS_PER_CLOCK_CYCLE = 250;

bool Player::handleInput(const InputEvent& event) {
    if(auto* clickevent = std::get_if<ClickEvent>(&event)) {
        if(clickevent->button == SDL_BUTTON_LEFT && SDL_PointInRect(&clickevent->clickPoint, &ffButton)) {
            fastForwardMultiplier = fastForwardMultiplier % Game::MAX_FAST_FORWARD + 1;
            return true;
        }
    }

    return false;
}

/*void renderClockHand(const Renderer& renderer, SDL_Point center, int w, int h, float angle, SDL_Color color) {
    
    SDL_FRect rect{float(center.x) - w/2.0f, float(center.y - h), float(w), float(h)};
    
    auto vCenter = glm::vec2(float(center.x), float(center.y));
    auto a = glm::vec2(rect.x, rect.y) - vCenter;
    auto v = vCenter + glm::rotate(a, angle);
    rect.x = v.x; rect.y = v.y;

    renderer.fillRect(rect, color, glm::degrees(angle));
}*/

void renderFastForward(int n, const Renderer& renderer, float x, float y, float w, float h, SDL_Color color) {
    SDL_FRect rect = {x - 3*n, y, w, h};
    for(int i=0; i<=n; ++i) {
        renderer.render("TRIS", rect.x, rect.y, rect, color);
        rect.x += w / n + 6;
    }
}

void Player::render(const Renderer& renderer, int currentTick) {
    auto screen = renderer.getScreenViewportRect();
    auto text = std::format("${}", cash);
    auto bounds = renderer.getTextBounds(text, 36);
    
    auto rect = SDL_Rect {screen.w - int(bounds.w) - 202, 0, int(bounds.w) + 192, 92};
    renderer.fillRect(rect, SDL_Color{0xE0, 0xE0, 0xE0, SDL_ALPHA_OPAQUE});

    renderer.renderText(text, screen.w - bounds.w - 192, 28, 36, Aligment::LEFT, SDL_BLACK);

    /*float minuteAngle = 2*M_PI / TICKS_PER_CLOCK_CYCLE * (currentTick % TICKS_PER_CLOCK_CYCLE);
    renderClockHand(renderer, SDL_Point(screen.w-56, 46), 2, 30, minuteAngle, SDL_SILVER);

    float hourAngle = 2*M_PI / (24*TICKS_PER_CLOCK_CYCLE) * (currentTick % (24*TICKS_PER_CLOCK_CYCLE));
    renderClockHand(renderer, SDL_Point(screen.w-56, 46), 4, 20, hourAngle, SDL_GOLD);

    rect.w = rect.h = 72;
    renderer.render("CLOCK", screen.w - 92, 10, toFRect(rect));*/

    ffButton = SDL_Rect { screen.w - 172, 20, 60, 52 };
    renderFastForward(fastForwardMultiplier, renderer, screen.w - 172, 20, 30, 52, SDL_BLACK);
}

void Player::update() {
    difficulty += 1e-5; //MORE OR LESS +50% PER HOUR 0,00001 * 15 * 3600 = 0,54
}

PlayerSave Player::serialize() const {
    PlayerSave save;

    save.cash = this->cash;
    save.difficulty = this->difficulty;
    save.fastForwardMultiplier = this->fastForwardMultiplier;
    save.stats = this->stats;

    return save;
}

void Player::deserialize(const PlayerSave& save) {
    this->cash = save.cash;
    this->difficulty = save.difficulty;
    this->fastForwardMultiplier = save.fastForwardMultiplier;
    this->stats = save.stats;
}
