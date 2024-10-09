#pragma once

#include <vector>

#include "../engine/Gradient.h"
#include "../game/Types.h"

class Camera;

namespace air {
    constexpr long PRICE_PER_FLIGHT = 1;

    constexpr Gradient FULL_GRADIENT = {{0, 0xff, 0, SDL_ALPHA_OPAQUE}, {0xff, 0, 0, SDL_ALPHA_OPAQUE}};
    constexpr float AIRPLANE_SCALE = 0.009f;

    constexpr size_t AIRPORT_LEVELS = 7;
    constexpr int AIRPORT_CAPACITY_PER_LEVEL[AIRPORT_LEVELS] = { 100, 200, 500, 1000, 2000, 5000, 10000 };
    constexpr int AIRPORT_UPGRADE_COST[AIRPORT_LEVELS-1] = { 100, 300, 900, 2700, 8100, 24300 };

    struct AirportData {
        int level = 0;
        float radius;

        std::vector<int> waiting;

        std::vector<int> routeIndexes;
    };

    float getRelativeRadius(float radius, float zoom);
    void renderAirport(const Camera& camera, const AirportData& airport, const City& city);
}
