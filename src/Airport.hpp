#pragma once

#include "map/Map.hpp"
#include "engine/Gradient.h"
#include "engine/InputEvent.h"

constexpr float PRICE_PER_METER_ROUTE = 0.0003;
constexpr float MTS_PER_TICK = 500;
constexpr Gradient FULL_GRADIENT = {{0x55, 0xBF, 0x40, SDL_ALPHA_OPAQUE}, {0x7D, 0x40, 0xBF, SDL_ALPHA_OPAQUE}};

class AirportManager;

struct Plane {
    float t;
    float speed;

    int routeIndex;

    int capacity;
};

struct Route {
    int a, b;
    float lenght;
    std::vector<glm::vec2> points;

    Route(int a, int b): a(a), b(b) {}
};


struct AirportData {
    int level;
    int radius;

    std::vector<size_t> routeIndexes;
};

class AirportManager {
public:
    bool handleInput(const InputEvent& event);
    void update(CitySpawner& citySpawner, Camera& camera, Player& player);
    void render(const Camera& camera, float frameProgress);

private:
    void updatePaths();

    void addAirport(City&& city);
    void addRoute(Route&& route, Player& player);
    void addPlane(Plane&& plane, Player& player);
    
    void renderAirport(const Camera& camera, int airportIndex) const;
    void renderPlane(const Camera& camera, const Plane& plane, float frameProgress) const;
    void renderRoute(const Camera& camera, const Route& route) const;

private:
    std::vector<City> cities;
    std::vector<AirportData> airports;
    std::vector<Route> routes;
    std::vector<Plane> planes;

    std::vector<std::vector<int>> networkAdjList;
    std::vector<std::vector<int>> parentTree;

    SDL_Point mousePos;
    bool leftDown = false, clicked = false;
    Route currentRoute {-1, -1};
    long currentPrice = 0;
};
