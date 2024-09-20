#include "Dialog.hpp"

#include "../engine/Gradient.h"
#include "../game/Camera.hpp"
#include "../map/Map.hpp"
#include "../Player.hpp"

#include <format>

void Dialog::renderButton(const Button& btn, const Camera& camera) const {
    auto color = btn.hovered? btn.hoverColor : btn.color;
    SDL_SetRenderDrawColor(camera.getSDL(), color.r, color.g, color.b, color.a);

    auto rect = btn.globalRect;
    SDL_RenderFillRect(camera.getSDL(), &rect);
    camera.renderText(btn.text, rect.x + rect.w/2, rect.y + (rect.h - btn.fontSize) / 2, btn.fontSize, FC_ALIGN_CENTER, btn.textColor);
}

UnlockCountryDialog::UnlockCountryDialog(const std::string& name, const std::string& code, Map& map, Player& player): 
    map(map), player(player), countryName(name), countryCode(code) 
{
    dialog = SDL_Rect {0, 0, 450, 300};
    
    yesButton.localRect = SDL_Rect{25, dialog.h -80, 180, 50};
    yesButton.globalRect = getBB(yesButton);
    yesButton.color = FC_MakeColor(46, 128, 42, SDL_ALPHA_OPAQUE);
    yesButton.hoverColor = FC_MakeColor(36, 102, 33, SDL_ALPHA_OPAQUE);
    yesButton.text = "YES";
    yesButton.fontSize = 40;

    noButton.localRect = SDL_Rect{dialog.w - 205, dialog.h -80, 180, 50};
    noButton.globalRect = getBB(noButton);
    noButton.color = FC_MakeColor(128, 42, 42, SDL_ALPHA_OPAQUE);
    noButton.hoverColor = FC_MakeColor(102, 34, 34, SDL_ALPHA_OPAQUE);
    noButton.text = "NO";
    noButton.fontSize = 40;
}

void UnlockCountryDialog::render(const Camera& camera) {
    auto viewport = camera.getScreenViewportRect();
    SDL_SetRenderDrawColor(camera.getSDL(), 0, 0, 0, 100);
    SDL_SetRenderDrawBlendMode(camera.getSDL(), SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(camera.getSDL(), &viewport);

    dialog.x = viewport.w/2 - dialog.w/2;
    dialog.y = viewport.h/2 - dialog.h/2;
    yesButton.globalRect = getBB(yesButton);
    noButton.globalRect = getBB(noButton);
    
    SDL_SetRenderDrawColor(camera.getSDL(), 100, 100, 100, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(camera.getSDL(), &dialog);

    renderButton(yesButton, camera);
    renderButton(noButton, camera);

    auto text = std::format("Buy {}?\nPrice: ${}", countryName, long(double(DEFAULT_CITY_PRICE) * player.getDifficulty()));
    camera.renderText(text, dialog.x + dialog.w/2, dialog.y + 10, 32, FC_ALIGN_CENTER, SDL_WHITE);
}

bool UnlockCountryDialog::handleInput(const InputEvent& event) {
    if(auto* clickevent = std::get_if<ClickEvent>(&event)) {
        if(SDL_PointInRect(&clickevent->clickPoint, &dialog) == SDL_FALSE) {
            die = true;
        }

        if(yesButton.hovered) {
            map.unlockCountry(this->countryCode, player);
            die = true;
        }

        if(noButton.hovered) {
            die = true;
        }
    }

    if(auto* mouseevent = std::get_if<MouseMoveEvent>(&event)) {
        yesButton.hovered = SDL_PointInRect(&mouseevent->newPos, &yesButton.globalRect);
        noButton.hovered = SDL_PointInRect(&mouseevent->newPos, &noButton.globalRect);
    }

    return true;
}
