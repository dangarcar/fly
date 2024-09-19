#include "Player.hpp"

#include <format>
#include <SDL_FontCache.h>

#include "game/Camera.hpp"

void Player::handleInput(const InputEvent& event) {
    //TODO:::
}

void Player::render(const Camera& camera) {
    auto screen = camera.getScreenViewportRect();
    auto text = std::format("${}", cash);
    auto bounds = camera.getTextRenderer().getTextBounds(text, 36);
    
    auto rect = SDL_Rect {screen.w - bounds.w - 30, 0, bounds.w + 20, bounds.h + 20};
    SDL_SetRenderDrawColor(camera.getSDL(), 0xE0, 0xE0, 0xE0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(camera.getSDL(), &rect);

    camera.renderText(text, screen.w - bounds.w - 20, 10, 36, FC_ALIGN_LEFT, FC_MakeColor(0, 0, 0, SDL_ALPHA_OPAQUE));
}