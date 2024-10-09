#pragma once

#include "../Dialog.hpp"
#include "../../game/Types.h"

class Player;

namespace air {
    class AirportManager;
    class Route;
    class Plane;
};

class RouteDialog: public Dialog {
public:
    RouteDialog(int routeIndex, air::Route& route, Player& player, air::AirportManager& airManager, const std::vector<City>& cities);

    bool handleInput(const InputEvent& event) override;
    void render(const Camera& camera) override;

private:
    bool canBuy() const;
    bool canUpgrade() const;

private:
    int routeIndex;
    air::Route& route;
    Player& player;
    air::AirportManager& airManager;

    const std::vector<City>& cities;

    Button buyButton, upgradeButton, sellButton;
};
