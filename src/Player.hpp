#pragma once

#include "engine/InputEvent.h"

class Camera;
class Game;


struct Stats {
    long routes = 0;
    long planes = 0;
    long flights = 0;
    
    long passengersTotal = 0;
    long passengersArrived = 0;
    
    long countries = 0;
    long airports = 0;
    long population = 0;
    
    long moneySpent = 0;
    long moneyEarned = 0;
};

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
            stats.moneySpent += amount;
            return true;
        } else {
            return false;
        }
    }

    void earn(long amount) { 
        cash += amount;
        stats.moneyEarned += amount;
    }

    double getDifficulty() const { return difficulty; }
    int getFastForward() const { return fastForwardMultiplier; }

    Stats stats;

private:
    long cash = INITIAL_CASH;
    double difficulty = 1.0;

    int fastForwardMultiplier = 1;
    SDL_Rect ffButton;
};
