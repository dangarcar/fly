#include "Dialog.hpp"

#include "../game/Camera.hpp"

void Dialog::render(const Camera& camera) {
    auto viewport = camera.getScreenViewportRect();
    SDL_SetRenderDrawColor(camera.getSDL(), 0, 0, 0, 100);
    SDL_SetRenderDrawBlendMode(camera.getSDL(), SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(camera.getSDL(), &viewport);

    dialog.x = viewport.w/2 - dialog.w/2;
    dialog.y = viewport.h/2 - dialog.h/2;

    SDL_SetRenderDrawColor(camera.getSDL(), 100, 100, 100, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(camera.getSDL(), &dialog);
}

bool Dialog::handleInput(const InputEvent& event) {
    if(auto* clickevent = std::get_if<ClickEvent>(&event)) {
        if(clickevent->button == SDL_BUTTON_LEFT && SDL_PointInRect(&clickevent->clickPoint, &dialog) == SDL_FALSE) {
            die = true;
            return true;
        }
    }

    return false;
}

void Button::render(const Camera& camera, SDL_Rect parent) {
    globalRect = localRect;
    globalRect.x += parent.x;
    globalRect.y += parent.y;

    auto c = hovered? hoverColor : color;
    if(disabled) 
        c = disabledColor;
    SDL_SetRenderDrawColor(camera.getSDL(), c.r, c.g, c.b, c.a);

    SDL_RenderFillRect(camera.getSDL(), &globalRect);
    camera.renderText(text, globalRect.x + globalRect.w/2, globalRect.y + (globalRect.h - fontSize) / 2, fontSize, FC_ALIGN_CENTER, textColor);
}