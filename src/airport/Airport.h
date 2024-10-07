#pragma once

#include <vector>

#include "../engine/Gradient.h"
#include "../game/Types.h"

constexpr long PRICE_PER_FLIGHT = 1;
constexpr float PRICE_PER_METER_ROUTE = 0.0003;

constexpr Gradient FULL_GRADIENT = {{0, 0xff, 0, SDL_ALPHA_OPAQUE}, {0xff, 0, 0, SDL_ALPHA_OPAQUE}};
constexpr float AIRPLANE_SCALE = 0.009f;

constexpr size_t PLANE_LEVELS = 7;
constexpr int PLANE_CAPACITY_PER_LEVEL[PLANE_LEVELS] = { 30, 50, 100, 200, 300, 500, 1000 };
constexpr int PLANE_UPGRADE_COST[PLANE_LEVELS-1] = { 100, 200, 400, 800, 1600, 3200 };
constexpr const char* PLANE_TEXTURE_PER_LEVEL[PLANE_LEVELS] = { "PLANE", "PLANE", "PLANE", "PLANE", "PLANE", "PLANE", "PLANE" };
constexpr float MTS_PER_TICK_PER_LEVEL[PLANE_LEVELS] = { 800, 1000, 1200, 1400, 1600, 1800, 2000 };

constexpr size_t AIRPORT_LEVELS = 7;
constexpr int AIRPORT_CAPACITY_PER_LEVEL[AIRPORT_LEVELS] = { 100, 200, 500, 1000, 2000, 5000, 10000 };
constexpr int AIRPORT_UPGRADE_COST[AIRPORT_LEVELS-1] = { 100, 300, 900, 2700, 8100, 24300 };

struct Plane {
    int level = 0;
    
    float t;
    float speed;

    int routeIndex;

    int people = 0;
    std::vector<int> pass;
};

struct Route {
    int a, b;
    float lenght;
    std::vector<glm::vec2> points;

    Route(int a, int b): a(a), b(b) {}
};

struct AirportData {
    int level = 0;
    float radius;

    int people = 0;
    std::vector<int> waiting;
    
    std::vector<int> routeIndexes;
};
