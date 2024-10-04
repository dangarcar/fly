#pragma once

#include "Dialog.hpp"

class Plane;

class PlaneDialog: public Dialog {
public:
    PlaneDialog(Plane& plane, Player& player);

    bool handleInput(const InputEvent& event) override;
    void render(const Camera& camera) override;

private:
    Plane& plane;
    Player& player;
};