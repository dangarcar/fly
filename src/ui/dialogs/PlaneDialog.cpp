#include "PlaneDialog.hpp"

#include <format>

#include "../../game/Camera.hpp"
#include "../../airport/AirportManager.hpp"
#include "../../Player.hpp"

std::string cutNCharacters(std::string s, int n) {
    if(int(s.length()) > n) {
        s = s.substr(0, n-1);
        if(s.back() == ' ')
            s = s.substr(0, n-2);
        s = s.append("...");
    }

    return s;
}

PlaneDialog::PlaneDialog(Plane& plane, Player& player, Route& route, const std::vector<City>& cities):
    plane(plane), player(player), route(route), cities(cities)
{
    dialog = SDL_Rect {0, 0, 300, 300};

    upgradeButton.localRect = SDL_Rect {20, 238, 260, 42};
    upgradeButton.color = hexCodeToColor("#2e802a");
    upgradeButton.hoverColor = hexCodeToColor("#246621");
    upgradeButton.disabledColor = SDL_SILVER;
    upgradeButton.fontSize = 28;
    upgradeButton.textColor = SDL_WHITE;
    upgradeButton.text = "UPGRADE";
}

void PlaneDialog::render(const Camera& camera) {
    Dialog::render(camera);

    //RENDER TITLE
    auto nameA = cutNCharacters(cities[route.a].name, 10), nameB = cutNCharacters(cities[route.b].name, 10);
    auto text = std::format("{} - {}", nameA, nameB);
    camera.renderText(text, dialog.x + dialog.w/2, dialog.y + 10, 28, FC_ALIGN_CENTER, SDL_WHITE);

    //RENDER BAR WITH FILLED PERCENTAGE
    auto rect = SDL_Rect { dialog.x + 20, dialog.y + dialog.h - 100, dialog.w - 40, 20 };
    SDL_SetRenderDrawColor(camera.getSDL(), 20, 20, 20, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(camera.getSDL(), &rect);

    rect = {rect.x + 5, rect.y + 5, rect.w - 10, rect.h - 10};
    SDL_SetRenderDrawColor(camera.getSDL(), 140, 140, 140, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(camera.getSDL(), &rect);

    auto fillPercentage = float(plane.people) / PLANE_CAPACITY_PER_LEVEL[plane.level];
    rect.w *= fillPercentage;
    assert(fillPercentage <= 1);
    auto color = FULL_GRADIENT.getColor(fillPercentage);
    SDL_SetRenderDrawColor(camera.getSDL(), 0, 200, 100, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(camera.getSDL(), &rect);

    auto& t = camera.getTextureManager().getTexture(PLANE_TEXTURE_PER_LEVEL[plane.level]);
    t.setColorMod(color);
    t.renderCenter(*camera.getSDL(), dialog.x + dialog.w/2, dialog.y + 120, 128.0f/t.getWidth(), 0);

    text = std::format("{}/{}", plane.people, PLANE_CAPACITY_PER_LEVEL[plane.level]);
    camera.renderText(text, dialog.x + dialog.w - 25, dialog.y + dialog.h - 124, 24, FC_ALIGN_RIGHT, SDL_WHITE);

    upgradeButton.disabled = plane.level+1 >= int(PLANE_LEVELS) || player.getCash() < PLANE_UPGRADE_COST[plane.level];
    if(plane.level+1 >= int(PLANE_LEVELS))
        upgradeButton.text = "CAN'T UPGRADE MORE";
    else if(player.getCash() < PLANE_UPGRADE_COST[plane.level])
        upgradeButton.text = "NOT ENOUGH MONEY";
    else
        upgradeButton.text = std::format("UPGRADE FOR ${}", PLANE_UPGRADE_COST[plane.level]);

    upgradeButton.globalRect = getBB(upgradeButton);
    Dialog::renderButton(upgradeButton, camera);
}

bool PlaneDialog::handleInput(const InputEvent& event) {
    if(Dialog::handleInput(event))
        return true;
    
    if(auto* clickevent = std::get_if<ClickEvent>(&event)) {
        if(clickevent->button == SDL_BUTTON_LEFT && upgradeButton.hovered) {
            if(!upgradeButton.disabled) {
                player.spend(PLANE_UPGRADE_COST[plane.level]);
                plane.level++;
                plane.speed = (plane.speed<0? -1:1) * MTS_PER_TICK_PER_LEVEL[plane.level] / route.lenght;
            }
        }
    }

    if(auto* moveevent = std::get_if<MouseMoveEvent>(&event)) {
        upgradeButton.hovered = SDL_PointInRect(&moveevent->newPos, &upgradeButton.globalRect);
        return false;
    }

    return true;
}