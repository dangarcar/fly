#include "Dialog.hpp"

#include "../engine/Gradient.h"
#include "../game/Camera.hpp"

#include <format>

void UnlockCountryDialog::render(const Camera& camera) {
    auto viewport = camera.getScreenViewportRect();
    SDL_SetRenderDrawColor(camera.getSDL(), 0, 0, 0, 100);
    SDL_SetRenderDrawBlendMode(camera.getSDL(), SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(camera.getSDL(), &viewport);

    dialog.x = viewport.w/2 - dialog.w/2;
    dialog.y = viewport.h/2 - dialog.h/2;
    SDL_SetRenderDrawColor(camera.getSDL(), 100, 100, 100, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(camera.getSDL(), &dialog);

    auto yesButton = FC_MakeRect(dialog.x + 25, dialog.y + dialog.h - 80, 180, 50);
    SDL_SetRenderDrawColor(camera.getSDL(), 46, 128, 42, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(camera.getSDL(), &yesButton);
    camera.renderText("YES", yesButton.x + yesButton.w/2, yesButton.y + 5, 40, FC_ALIGN_CENTER, SDL_WHITE);

    auto noButton = FC_MakeRect(dialog.x + dialog.w - 205, dialog.y + dialog.h - 80, 180, 50);
    SDL_SetRenderDrawColor(camera.getSDL(), 128, 42, 42, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(camera.getSDL(), &noButton);
    camera.renderText("NO", noButton.x + noButton.w/2, noButton.y + 5, 40, FC_ALIGN_CENTER, SDL_WHITE);

    auto text = std::format("Buy {}?", countryName);
    camera.renderText(text, dialog.x + dialog.w/2, dialog.y + 10, 32, FC_ALIGN_CENTER, SDL_WHITE);

    
}
