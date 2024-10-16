#include "Dialog.hpp"

#include "../game/Camera.hpp"

void Dialog::render(const Renderer& renderer) {
    auto viewport = renderer.getScreenViewportRect();
    SDL_SetRenderDrawColor(&renderer.getSDL(), 0, 0, 0, 100);
    SDL_SetRenderDrawBlendMode(&renderer.getSDL(), SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(&renderer.getSDL(), &viewport);

    dialog.x = viewport.w/2 - dialog.w/2;
    dialog.y = viewport.h/2 - dialog.h/2;

    SDL_SetRenderDrawColor(&renderer.getSDL(), background.r, background.g, background.b, background.a);
    SDL_RenderFillRect(&renderer.getSDL(), &dialog);
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

void Button::render(const Renderer& renderer, SDL_Rect parent) {
    globalRect = localRect;
    globalRect.x += parent.x;
    globalRect.y += parent.y;

    auto c = hovered? hoverColor : color;
    if(disabled) 
        c = disabledColor;
    SDL_SetRenderDrawColor(&renderer.getSDL(), c.r, c.g, c.b, c.a);

    SDL_RenderFillRect(&renderer.getSDL(), &globalRect);
    renderer.renderText(text, globalRect.x + globalRect.w/2, globalRect.y + (globalRect.h - fontSize) / 2, fontSize, FC_ALIGN_CENTER, textColor);
}