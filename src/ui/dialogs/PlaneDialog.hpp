#pragma once

#include "../Dialog.hpp"
#include "../../game/Types.h"

class Plane;
class Route;

class PlaneDialog: public Dialog {
public:
    PlaneDialog(Plane& plane, Player& player, Route& route, const std::vector<City>& cities);

    bool handleInput(const InputEvent& event) override;
    void render(const Camera& camera) override;

private:
    Plane& plane;
    Player& player;
    Route& route;

    const std::vector<City>& cities;

    Button upgradeButton;
};
