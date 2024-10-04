#include "AirportDialog.hpp"

#include <format>

#include "../../game/Camera.hpp"
#include "../../airport/AirportManager.hpp"
#include "../../Player.hpp"

AirportDialog::AirportDialog(AirportData& airport, const City& city, Player& player, const std::vector<City>& cities):
    airport(airport), city(city), player(player), cities(cities)
{
    dialog = SDL_Rect {0, 0, 600, 400};

    upgradeButton.localRect = SDL_Rect {15, 253, 270, 42};
    upgradeButton.color = hexCodeToColor("#2e802a");
    upgradeButton.hoverColor = hexCodeToColor("#246621");
    upgradeButton.disabledColor = SDL_SILVER;
    upgradeButton.fontSize = 28;
    upgradeButton.textColor = SDL_WHITE;
    upgradeButton.text = "UPGRADE";
}

void AirportDialog::render(const Camera& camera) {
    Dialog::render(camera);

    //RENDER TITLE
    camera.renderText(city.name, dialog.x + dialog.w/2, dialog.y + 10, 42, FC_ALIGN_CENTER, SDL_WHITE);

    auto text = std::format("Population: {}", city.population);
    camera.renderText(text, dialog.x + 20, dialog.y + 60, 24, FC_ALIGN_LEFT, SDL_WHITE);

    //RENDER BAR WITH FILLED PERCENTAGE
    auto rect = SDL_Rect { dialog.x + 20, dialog.y + dialog.h - 60, dialog.w - 40, 40 };
    SDL_SetRenderDrawColor(camera.getSDL(), 20, 20, 20, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(camera.getSDL(), &rect);

    rect = {rect.x + 5, rect.y + 5, rect.w - 10, rect.h - 10};
    SDL_SetRenderDrawColor(camera.getSDL(), 140, 140, 140, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(camera.getSDL(), &rect);

    auto fillPercentage = float(airport.people) / AIRPORT_CAPACITY_PER_LEVEL[airport.level];
    rect.w *= std::min(1.0f, fillPercentage);
    if(fillPercentage < 1)
        SDL_SetRenderDrawColor(camera.getSDL(), 0, 200, 100, SDL_ALPHA_OPAQUE);
    else
        SDL_SetRenderDrawColor(camera.getSDL(), 250, 50, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(camera.getSDL(), &rect);

    text = std::format("{}/{}", airport.people, AIRPORT_CAPACITY_PER_LEVEL[airport.level]);
    auto textColor = fillPercentage<1? SDL_WHITE : FC_MakeColor(150,0,0,255);
    camera.renderText(text, dialog.x + dialog.w - 25, dialog.y + dialog.h - 86, 24, FC_ALIGN_RIGHT, textColor);

    //RENDER TOP DESTINATIONS
    rect = SDL_Rect { dialog.x + dialog.w/2 + 10, dialog.y + 60, dialog.w/2 - 30, 235 };
    SDL_SetRenderDrawColor(camera.getSDL(), 50, 50, 50, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(camera.getSDL(), &rect);

    camera.renderText("Top destinations", dialog.x + 3*dialog.w/4, dialog.y + 67, 32, FC_ALIGN_CENTER, SDL_WHITE);

    auto dest = getFrequentDestinations();
    for(int i=0; i<int(dest.size()); ++i) {
        text = dest[i].first;
        if(text.length() > 18) {
            text = text.substr(0, 17);
            if(text.back() == ' ')
                text = text.substr(0, 16);
            text = text.append("...");
        }
        camera.renderText(text, dialog.x + dialog.w/2 + 17, dialog.y + 106 + 26*i, 26, FC_ALIGN_LEFT, SDL_WHITE);
        text = std::format("{}", dest[i].second);
        camera.renderText(text, dialog.x + dialog.w - 27, dialog.y + 106 + 26*i, 26, FC_ALIGN_RIGHT, SDL_GOLD);
    }

    //RENDER LEVEL UPGRADES
    auto& t = camera.getTextureManager().getTexture("CIRCLE");
    t.setColorMod(SDL_GOLD);
    t.renderCenter(*camera.getSDL(), dialog.x + dialog.w/4, dialog.y + 168, 134.0f / t.getWidth(), 0);
    text = std::to_string(airport.level + 1);
    camera.renderText(text, dialog.x + dialog.w/4, dialog.y + 104, 128, FC_ALIGN_CENTER, SDL_WHITE);

    upgradeButton.disabled = !canUpgrade();
    if(canUpgrade())
        upgradeButton.text = std::format("UPGRADE FOR ${}", AIRPORT_UPGRADE_COST[airport.level]);
    else if(player.getCash() < AIRPORT_UPGRADE_COST[airport.level])
        upgradeButton.text = "NOT ENOUGH MONEY";
    else
        upgradeButton.text = "CAN'T UPGRADE MORE";

    upgradeButton.globalRect = getBB(upgradeButton);
    Dialog::renderButton(upgradeButton, camera);
}

bool AirportDialog::handleInput(const InputEvent& event) {
    if(Dialog::handleInput(event))
        return true;
    
    if(auto* clickevent = std::get_if<ClickEvent>(&event)) {
        if(clickevent->button == SDL_BUTTON_LEFT && upgradeButton.hovered) {
            if(!upgradeButton.disabled) {
                player.spend(AIRPORT_UPGRADE_COST[airport.level]);
                airport.level++;
            }
        }
    }

    if(auto* moveevent = std::get_if<MouseMoveEvent>(&event)) {
        upgradeButton.hovered = SDL_PointInRect(&moveevent->newPos, &upgradeButton.globalRect);
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
    return airport.level < int(AIRPORT_LEVELS-1) &&
        AIRPORT_UPGRADE_COST[airport.level] <= player.getCash();
}
