#pragma once

#include <unordered_map>

#include "../engine/InputEvent.h"
#include "../engine/Serializable.h"

#include "AgentSpawner.hpp"
#include "Airport.hpp"
#include "Route.hpp"

class Camera;
class Player;
class Map;
class CitySpawner;
class UIManager;

namespace air {

    struct CurrentRoute {
        Route route = {-1, -1};
        long price = 0;
        SDL_Color color = {0xff, 0xff, 0xff, 0xff};
    };

    struct AirportSave {
        std::vector<City> cities; //Not serialized
        std::vector<air::AirportData> airports;
        std::vector<air::Route> routes;
        std::vector<std::vector<int>> networkAdjList;
        std::vector<std::vector<int>> routeGrids;
    };

    class AirportManager: Serializable<AirportSave> {
    public:
        void start(const std::unordered_map<std::string, Country>& countries);

        bool handleInput(const InputEvent& event, Player& player, UIManager& uiManager);
        void update(CitySpawner& citySpawner, Camera& camera, Player& player, UIManager& uiManager);
        void render(const Camera& camera, float frameProgress);

        AirportSave serialize() const override;
        void deserialize(const AirportSave& save) override;

        //DOES NOT COST MONEY
        void addPlane(Route& route, Player& player);

        void deleteRoute(int routeIndex, Player& player);

    private:
        void updatePaths();

        void addAirport(City&& city, Player& player);
        bool addRoute(Route&& route, Player& player, const Camera& camera);

        int getNextAirport(int src, int target) { return parentTree[target][src]; }
        //Player can be null
        void landPlane(Player* player, Plane& plane, Route& route, bool inverted);

    private:
        std::vector<City> cities;
        std::vector<AirportData> airports;
        
        std::vector<Route> routes;
        std::vector<std::vector<int>> routeGrids;

        std::vector<std::vector<int>> networkAdjList;
        std::vector<std::vector<int>> parentTree;

        AgentSpawner agentSpawner;
        CurrentRoute currentRoute;

        AirportRenderer airportRenderer;

        int clickedAirport = -1, clickedRoute = -1;

        SDL_Point mousePos;
        bool leftDown = false;
    };

};
