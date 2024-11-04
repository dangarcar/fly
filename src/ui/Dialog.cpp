#include "Dialog.hpp"

#include "../game/Camera.hpp"

void Dialog::render(const Renderer& renderer) {
    auto viewport = renderer.getScreenViewportRect();
    renderer.fillRect(viewport, SDL_Color{0, 0, 0, 100});
    dialog.x = viewport.w/2 - dialog.w/2;
    dialog.y = viewport.h/2 - dialog.h/2;

    renderer.fillRect(dialog, background);
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
    renderer.fillRect(globalRect, c);
    
    renderer.renderText(text, globalRect.x + globalRect.w/2, globalRect.y + (globalRect.h - fontSize) / 2, fontSize, Aligment::CENTER, textColor);
}