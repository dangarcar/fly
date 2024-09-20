#pragma once

#include "engine/InputEvent.h"

class Camera;

class Player {
private:
    static constexpr long INITIAL_CASH = 3000;
    //static constexpr long INITIAL_CASH = 1000000000;

public:
    void handleInput(const InputEvent& event);
    void render(const Camera& camera);
    void update();

    long getCash() const { return cash; }
    void spend(long amount) { cash -= amount; }
    void earn(long amount) { cash += amount; }

    double getDifficulty() const { return difficulty; }

private:
    long cash = INITIAL_CASH;
    double difficulty = 1.0;
};
