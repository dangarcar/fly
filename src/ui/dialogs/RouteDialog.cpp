#include "RouteDialog.hpp"

#include "../../engine/Renderer.hpp"
#include "../../Player.hpp"
#include "../../airport/AirportManager.hpp"
#include "../../engine/Utils.h"

using air::AirportManager, air::Route, air::Plane;

bool RouteDialog::canUpgrade() const {
    long upgradePrice = route.planes.size() * air::PLANE_UPGRADE_COST[route.level];
    return route.level < int(air::ROUTE_LEVELS-1) && upgradePrice <= player.getCash();
}

bool RouteDialog::canBuy() const {
    long buyPrice = air::PLANE_PRICE_PER_LEVEL[route.level];
    return route.planes.size() < air::MAX_PLANES_PER_ROUTE && buyPrice <= player.getCash();
}

RouteDialog::RouteDialog(int routeIndex, Route& route, Player& player, AirportManager& airManager, const std::vector<City>& cities): 
    routeIndex(routeIndex), route(route), player(player), airManager(airManager), cities(cities)
{
    dialog = SDL_Rect {0, 0, 540, 400};

    upgradeButton.localRect = SDL_Rect {20, 208, 500, 42};
    upgradeButton.color = SDL_CYAN;
    upgradeButton.hoverColor = SDL_DARK_CYAN;
    upgradeButton.fontSize = 28;

    buyButton.localRect = SDL_Rect {20, 260, 245, 42};
    buyButton.color = SDL_GREEN;
    buyButton.hoverColor = SDL_DARK_GREEN;
    buyButton.fontSize = 28;

    sellButton.localRect = SDL_Rect {275, 260, 245, 42};
    sellButton.color = SDL_RED;
    sellButton.hoverColor = SDL_DARK_RED;
    sellButton.fontSize = 28;
}

void RouteDialog::render(const Renderer& renderer) {
    Dialog::render(renderer);

    //RENDER TITLE
    auto nameA = cutNCharacters(cities[route.a].name, 15), nameB = cutNCharacters(cities[route.b].name, 15);
    auto text = std::format("{} - {}", nameA, nameB);
    renderer.renderText(text, dialog.x + dialog.w/2, dialog.y + 10, 36, FC_ALIGN_CENTER, SDL_WHITE);

    //RENDER PLANES
    auto& t = renderer.getTextureManager().getTexture(air::PLANE_TEXTURE_PER_LEVEL[route.level]);
    auto planesW = (route.planes.size() - 1) * 100;
    for(int i=0; i<int(route.planes.size()); ++i) {
        auto x = dialog.x + dialog.w/2 - planesW/2 + i*100;
        auto fillPercentage = float(route.planes[i].pass.size()) / air::PLANE_CAPACITY_PER_LEVEL[route.level];
        t.setColorMod(air::FULL_GRADIENT.getColor(fillPercentage));
        t.renderCenter(renderer.getSDL(), x, dialog.y + 120, 90.0f/t.getWidth(), 0);
    }

    //BUY BUTTON
    buyButton.setDisabled(!canBuy());
    if(route.planes.size() >= air::MAX_PLANES_PER_ROUTE) {
        buyButton.text = "CAN'T BUY MORE";
    } else {
        buyButton.text = std::format("BUY NEW FOR ${}", air::PLANE_PRICE_PER_LEVEL[route.level]);
    }
    buyButton.render(renderer, dialog);

    //UPGRADE BUTTON
    upgradeButton.setDisabled(!canUpgrade());
    if(route.level >= int(air::ROUTE_LEVELS-1)) {
        upgradeButton.text = "CAN'T UPGRADE MORE";
    } else {
        long upgradePrice = route.planes.size() * air::PLANE_UPGRADE_COST[route.level];
        upgradeButton.text = std::format("UPGRADE PLANES FOR ${}", upgradePrice);
    }
    upgradeButton.render(renderer, dialog);

    //SELL BUTTON
    long sellPrice = air::routePrice(route) * player.getDifficulty() * air::SELL_RETRIEVAL_RATIO;
    sellButton.text = std::format("SELL FOR ${}", sellPrice);
    sellButton.render(renderer, dialog);

    //RENDER FILL BAR
    auto total = air::PLANE_CAPACITY_PER_LEVEL[route.level] * route.planes.size();
    int passengers = 0;
    for(const auto& p: route.planes)
        passengers += p.pass.size();

    auto rect = SDL_Rect { dialog.x + 20, dialog.y + dialog.h - 60, dialog.w - 40, 40 };
    SDL_SetRenderDrawColor(&renderer.getSDL(), 20, 20, 20, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(&renderer.getSDL(), &rect);

    rect = {rect.x + 5, rect.y + 5, rect.w - 10, rect.h - 10};
    SDL_SetRenderDrawColor(&renderer.getSDL(), 140, 140, 140, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(&renderer.getSDL(), &rect);

    auto color = air::FULL_GRADIENT.getColor(float(passengers) / total);
    rect.w *= std::min(1.0f, float(passengers)/total);
    SDL_SetRenderDrawColor(&renderer.getSDL(), color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(&renderer.getSDL(), &rect);

    text = std::format("{}/{}", passengers, total);
    renderer.renderText(text, dialog.x + dialog.w - 30, dialog.y + dialog.h - 52, 24, FC_ALIGN_RIGHT, SDL_WHITE);
}

bool RouteDialog::handleInput(const InputEvent& event) {
    if(Dialog::handleInput(event))
        return true;

    if(auto* clickevent = std::get_if<ClickEvent>(&event)) {
        if(clickevent->button == SDL_BUTTON_LEFT) {
            if(buyButton.isClickable()) {
                long buyPrice = air::PLANE_PRICE_PER_LEVEL[route.level];
                player.spend(buyPrice);
                
                airManager.addPlane(route, player);
            } 
            else if(upgradeButton.isClickable()) {
                long upgradePrice = route.planes.size() * air::PLANE_UPGRADE_COST[route.level];
                player.spend(upgradePrice);

                route.level++;
                for(auto& p: route.planes)
                    p.speed *= float(air::MTS_PER_TICK_PER_LEVEL[route.level]) / air::MTS_PER_TICK_PER_LEVEL[route.level-1];
            }
            else if(sellButton.isClickable()) {
                long sellPrice = air::routePrice(route) * player.getDifficulty() * air::SELL_RETRIEVAL_RATIO;
                player.earn(sellPrice);
                airManager.deleteRoute(routeIndex, player);
                die = true;
            }
        }
    }

    if(auto* moveevent = std::get_if<MouseMoveEvent>(&event)) {
        buyButton.updateHover(moveevent->newPos);
        upgradeButton.updateHover(moveevent->newPos);
        sellButton.updateHover(moveevent->newPos);
        return false;
    }

    return true;
}
