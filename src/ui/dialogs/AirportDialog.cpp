#include "AirportDialog.hpp"

#include <format>

#include "../../engine/Renderer.hpp"
#include "../../airport/AirportManager.hpp"
#include "../../Player.hpp"

AirportDialog::AirportDialog(air::AirportData& airport, const City& city, Player& player, const std::vector<City>& cities):
    airport(airport), city(city), player(player), cities(cities)
{
    dialog = SDL_Rect {0, 0, 600, 400};

    upgradeButton.localRect = SDL_Rect {15, 253, 270, 42};
    upgradeButton.color = SDL_GREEN;
    upgradeButton.hoverColor = SDL_DARK_GREEN;
    upgradeButton.fontSize = 28;
}

void AirportDialog::render(const Renderer& renderer) {
    Dialog::render(renderer);

    //RENDER TITLE
    renderer.renderText(city.name, dialog.x + dialog.w/2, dialog.y + 10, 42, FC_ALIGN_CENTER, SDL_WHITE);

    auto text = std::format("Population: {}", city.population);
    renderer.renderText(text, dialog.x + 20, dialog.y + 60, 24, FC_ALIGN_LEFT, SDL_WHITE);

    //RENDER BAR WITH FILLED PERCENTAGE
    auto rect = SDL_Rect { dialog.x + 20, dialog.y + dialog.h - 60, dialog.w - 40, 40 };
    SDL_SetRenderDrawColor(&renderer.getSDL(), 20, 20, 20, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(&renderer.getSDL(), &rect);

    rect = {rect.x + 5, rect.y + 5, rect.w - 10, rect.h - 10};
    SDL_SetRenderDrawColor(&renderer.getSDL(), 140, 140, 140, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(&renderer.getSDL(), &rect);

    auto fillPercentage = float(airport.waiting.size()) / air::AIRPORT_CAPACITY_PER_LEVEL[airport.level];
    rect.w *= std::min(1.0f, fillPercentage);
    if(fillPercentage < 1)
        SDL_SetRenderDrawColor(&renderer.getSDL(), 0, 200, 100, SDL_ALPHA_OPAQUE);
    else
        SDL_SetRenderDrawColor(&renderer.getSDL(), 250, 50, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(&renderer.getSDL(), &rect);

    text = std::format("{}/{}", airport.waiting.size(), air::AIRPORT_CAPACITY_PER_LEVEL[airport.level]);
    auto textColor = fillPercentage<1? SDL_WHITE : FC_MakeColor(150,0,0,255);
    renderer.renderText(text, dialog.x + dialog.w - 25, dialog.y + dialog.h - 86, 24, FC_ALIGN_RIGHT, textColor);

    //RENDER TOP DESTINATIONS
    rect = SDL_Rect { dialog.x + dialog.w/2 + 10, dialog.y + 60, dialog.w/2 - 30, 235 };
    SDL_SetRenderDrawColor(&renderer.getSDL(), 50, 50, 50, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(&renderer.getSDL(), &rect);

    renderer.renderText("Top destinations", dialog.x + 3*dialog.w/4, dialog.y + 67, 32, FC_ALIGN_CENTER, SDL_WHITE);

    auto dest = getFrequentDestinations();
    for(int i=0; i<int(dest.size()); ++i) {
        text = dest[i].first;
        if(text.length() > 18) {
            text = text.substr(0, 17);
            if(text.back() == ' ')
                text = text.substr(0, 16);
            text = text.append("...");
        }
        renderer.renderText(text, dialog.x + dialog.w/2 + 17, dialog.y + 106 + 26*i, 26, FC_ALIGN_LEFT, SDL_WHITE);
        text = std::format("{}", dest[i].second);
        renderer.renderText(text, dialog.x + dialog.w - 27, dialog.y + 106 + 26*i, 26, FC_ALIGN_RIGHT, SDL_GOLD);
    }

    //RENDER LEVEL UPGRADES
    auto& t = renderer.getTextureManager().getTexture("CIRCLE");
    t.setColorMod(SDL_GOLD);
    t.renderCenter(renderer.getSDL(), dialog.x + dialog.w/4, dialog.y + 168, 134.0f / t.getWidth(), 0);
    text = std::to_string(airport.level + 1);
    renderer.renderText(text, dialog.x + dialog.w/4, dialog.y + 104, 128, FC_ALIGN_CENTER, SDL_WHITE);

    upgradeButton.setDisabled(!canUpgrade());
    if(canUpgrade())
        upgradeButton.text = std::format("UPGRADE FOR ${}", air::AIRPORT_UPGRADE_COST[airport.level]);
    else {
        if(airport.level+1 >= int(air::AIRPORT_LEVELS))
            upgradeButton.text = "CAN'T UPGRADE MORE";
        else
            upgradeButton.text = "NOT ENOUGH MONEY";
    }

    upgradeButton.render(renderer, dialog);
}

bool AirportDialog::handleInput(const InputEvent& event) {
    if(Dialog::handleInput(event))
        return true;
    
    if(auto* clickevent = std::get_if<ClickEvent>(&event)) {
        if(clickevent->button == SDL_BUTTON_LEFT) {
            if(upgradeButton.isClickable()) {
                player.spend(air::AIRPORT_UPGRADE_COST[airport.level]);
                airport.level++;
            }
        }
    }

    if(auto* moveevent = std::get_if<MouseMoveEvent>(&event)) {
        upgradeButton.updateHover(moveevent->newPos);
        return false;
    }

    return true;
}

std::vector<std::pair<std::string, int>> AirportDialog::getFrequentDestinations() const {
    std::unordered_map<std::string, int> count;

    for(auto p: airport.waiting) {
        count[cities[p].name]++;
    }

    std::vector<std::pair<std::string, int>> result(count.begin(), count.end());
    std::sort(result.begin(), result.end(), [](auto& a, auto& b){
        return a.second > b.second;
    });

    return {result.begin(), result.begin() + std::min(7ULL, result.size())};
}

bool AirportDialog::canUpgrade() const {
    return airport.level < int(air::AIRPORT_LEVELS-1) &&
        air::AIRPORT_UPGRADE_COST[airport.level] <= player.getCash();
}
