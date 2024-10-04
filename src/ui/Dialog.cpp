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

void Dialog::renderButton(const Button& btn, const Camera& camera) const {
    auto color = btn.hovered? btn.hoverColor : btn.color;
    if(btn.disabled) color = btn.disabledColor;
    SDL_SetRenderDrawColor(camera.getSDL(), color.r, color.g, color.b, color.a);

    auto rect = btn.globalRect;
    SDL_RenderFillRect(camera.getSDL(), &rect);
    camera.renderText(btn.text, rect.x + rect.w/2, rect.y + (rect.h - btn.fontSize) / 2, btn.fontSize, FC_ALIGN_CENTER, btn.textColor);
}