#pragma once

#include "../engine/InputEvent.h"

#include "AgentSpawner.hpp"
#include "Airport.h"

class Camera;
class Player;
class Map;
class CitySpawner;
class UIManager;

struct CurrentRoute {
    Route route = {-1, -1};
    long price = 0;
    SDL_Color color = {0xff, 0xff, 0xff, 0xff};
};

class AirportManager {
public:
    bool handleInput(const InputEvent& event);
    void update(CitySpawner& citySpawner, Camera& camera, Player& player, UIManager& uiManager);
    void render(const Camera& camera, float frameProgress) const ;

private:
    void updatePaths();

    void addAirport(City&& city);
    bool addRoute(Route&& route, Player& player);
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

    CurrentRoute currentRoute;

    SDL_Point mousePos;
    bool leftDown = false, clicked = false;
};
