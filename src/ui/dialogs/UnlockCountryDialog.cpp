#include "UnlockCountryDialog.hpp"

#include "../../engine/Renderer.hpp"
#include "../../engine/Gradient.h"
#include "../../map/Map.hpp"
#include "../../Player.hpp"

#include <format>

UnlockCountryDialog::UnlockCountryDialog(const std::string& name, const std::string& code, Map& map, Player& player): 
    map(map), player(player), countryName(name), countryCode(code) 
{
    dialog = SDL_Rect {0, 0, 550, 320};
    
    yesButton.localRect = SDL_Rect{25, dialog.h - 70, 230, 50};
    yesButton.color = SDL_GREEN;
    yesButton.hoverColor = SDL_DARK_GREEN;
    yesButton.text = "YES";
    yesButton.fontSize = 40;

    noButton.localRect = SDL_Rect{dialog.w - 255, dialog.h - 70, 230, 50};
    noButton.color = SDL_RED;
    noButton.hoverColor = SDL_DARK_RED;
    noButton.text = "NO";
    noButton.fontSize = 40;
}

void UnlockCountryDialog::render(const Renderer& renderer) {
    Dialog::render(renderer);

    yesButton.setDisabled( player.getCash() < DEFAULT_CITY_PRICE * player.getDifficulty() );
    yesButton.render(renderer, dialog);

    noButton.render(renderer, dialog);

    auto text = std::format("Buy {}?", countryName);
    renderer.renderText(text, dialog.x + dialog.w/2, dialog.y + 10, 32, FC_ALIGN_CENTER, SDL_WHITE);

    auto& cir = renderer.getTextureManager().getTexture("CIRCLE");
    cir.setColorMod(SDL_BLACK);
    auto rect = SDL_Rect { dialog.x + dialog.w/2 - 70, dialog.y + 52, 140, 140 };
    cir.render(*renderer.getSDL(), rect.x, rect.y, &rect);

    auto& t = renderer.getTextureManager().getTexture(countryCode);
    rect = SDL_Rect { dialog.x + dialog.w/2 - 64, dialog.y + 58, 128, 128 };
    t.render(*renderer.getSDL(), rect.x, rect.y, &rect);

    text = std::format("Price: ${}", long(double(DEFAULT_CITY_PRICE) * player.getDifficulty()));
    renderer.renderText(text, dialog.x + dialog.w/2, dialog.y + 207, 26, FC_ALIGN_CENTER, SDL_WHITE);
}

bool UnlockCountryDialog::handleInput(const InputEvent& event) {
    if(Dialog::handleInput(event))
        return true;
    
    if(auto* clickevent = std::get_if<ClickEvent>(&event)) {
        if(clickevent->button == SDL_BUTTON_LEFT) {
            if(yesButton.isClickable()) {
                map.unlockCountry(this->countryCode, player);
                die = true;
            }
            else if(noButton.isClickable()) {
                die = true;
            }
        }
    }

    if(auto* mouseevent = std::get_if<MouseMoveEvent>(&event)) {
        yesButton.updateHover(mouseevent->newPos);
        noButton.updateHover(mouseevent->newPos);
        return false;
    }

    return true;
}
