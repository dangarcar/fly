#pragma once

#include <vector>

#include "../engine/Gradient.h"
#include "../engine/InputEvent.h"
#include "../game/Types.h"

#include "AgentSpawner.hpp"

constexpr long PRICE_PER_FLIGHT = 1;

constexpr float PRICE_PER_METER_ROUTE = 0.0003;
constexpr Gradient FULL_GRADIENT = {{0, 0xff, 0, SDL_ALPHA_OPAQUE}, {0xff, 0, 0, SDL_ALPHA_OPAQUE}};

constexpr size_t PLANE_LEVELS = 1;
constexpr int PLANE_CAPACITY_PER_LEVEL[PLANE_LEVELS] = { 50 };
constexpr float MTS_PER_TICK_PER_LEVEL[PLANE_LEVELS] = { 5000 };

constexpr size_t AIRPORT_LEVELS = 1;
constexpr int AIRPORT_CAPACITY_PER_LEVEL[AIRPORT_LEVELS] = { 100 };

class Camera;
class Player;
class Map;
class CitySpawner;

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
    int radius;

    int people = 0;
    std::vector<int> waiting;
    
    std::vector<int> routeIndexes;
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
    void addPlane(int routeIndex, Player& player);
    
    void renderAirport(const Camera& camera, int airportIndex) const;
    void renderPlane(const Camera& camera, const Plane& plane, float frameProgress) const;
    void renderRoute(const Camera& camera, const Route& route) const;

    int getNextAirport(int src, int target) { return parentTree[target][src]; }
    void landPlane(Player& player, Plane& plane, int a, int b);

private:
    std::vector<City> cities;
    std::vector<AirportData> airports;
    std::vector<Route> routes;
    std::vector<Plane> planes;

    std::vector<std::vector<int>> networkAdjList;
    std::vector<std::vector<int>> parentTree;

    AgentSpawner agentSpawner;

    SDL_Point mousePos;
    bool leftDown = false, clicked = false;
    Route currentRoute {-1, -1};
    long currentPrice = 0;
};
