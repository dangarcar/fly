#pragma once

#include "PlayerCamera.hpp"

class Player {
private:
    //static constexpr long INITIAL_CASH = 1000;
    static constexpr long INITIAL_CASH = 1000000000;

public:
    Player(const PlayerCamera& playerCamera): camera(playerCamera) {}

    const PlayerCamera& getCamera() const { return camera; }

    void registerEvents(Event::EventManager& manager);
    void render(const Renderer& renderer);

    long getCash() const { return cash; }
    void spend(long amount) { cash -= amount; }
    void earn(long amount) { cash += amount; }

private:
    PlayerCamera camera;
    long cash = INITIAL_CASH;

};

void Player::registerEvents(Event::EventManager& manager) {
    camera.registerEvents(manager);
}

void Player::render(const Renderer& renderer) {
    auto screen = camera.getScreenViewportRect();
    auto text = std::format("${}", cash);
    auto bounds = renderer.getTextRenderer().getTextBounds(text, 36);
    
    auto rect = SDL_Rect {screen.w - bounds.w - 30, 0, bounds.w + 20, bounds.h + 20};
    SDL_SetRenderDrawColor(renderer.getSDL(), 0xE0, 0xE0, 0xE0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer.getSDL(), &rect);

    renderer.renderText(text, screen.w - bounds.w - 20, 10, 36, FC_ALIGN_LEFT, FC_MakeColor(0, 0, 0, SDL_ALPHA_OPAQUE));
}
