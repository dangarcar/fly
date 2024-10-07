#pragma once

#include "engine/InputEvent.h"

class Camera;
class Game;

class Player {
private:
    static constexpr long INITIAL_CASH = 30000;
    //static constexpr long INITIAL_CASH = 1000000000;

public:
    bool handleInput(const InputEvent& event);
    void render(const Camera& camera, int currentTick);
    void update();

    long getCash() const { return cash; }
    bool spend(long amount) { 
        if(amount <= cash) {
            cash -= amount;
            return true;
        } else {
            return false;
        }
    }
    void earn(long amount) { cash += amount; }

    double getDifficulty() const { return difficulty; }
    int getFastForward() const { return fastForwardMultiplier; }

private:
    long cash = INITIAL_CASH;
    double difficulty = 1.0;

    int fastForwardMultiplier = 1;
    SDL_Rect ffButton;
};
