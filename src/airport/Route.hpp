#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "../game/Types.h"
#include "../engine/Gradient.h"

class Camera;

namespace air {

    constexpr float PRICE_PER_METER_ROUTE = 0.0003;
    constexpr float SELL_RETRIEVAL_RATIO = 0.67;

    constexpr size_t ROUTE_LEVELS = 3;
    constexpr int MAX_PLANES_PER_ROUTE = 5;
    constexpr SDL_Color ROUTE_COLOR_BY_LEVEL[ROUTE_LEVELS] = { SDL_WHITE, SDL_SILVER, SDL_GOLD };

    constexpr int PLANE_CAPACITY_PER_LEVEL[ROUTE_LEVELS] = { 50, 100, 200 };
    constexpr int PLANE_PRICE_PER_LEVEL[ROUTE_LEVELS] = { 100, 200, 300 };
    constexpr int PLANE_UPGRADE_COST[ROUTE_LEVELS-1] = { 200, 400 };
    constexpr const char* PLANE_TEXTURE_PER_LEVEL[ROUTE_LEVELS] = { "PLANE0", "PLANE1", "PLANE2" };
    constexpr float MTS_PER_TICK_PER_LEVEL[ROUTE_LEVELS] = { 800, 1200, 1600 };

    constexpr int ROUTE_GRID_HEIGHT = 128, ROUTE_GRID_WIDTH = 128;

    struct Plane {    
        float t;
        float speed;

        std::vector<int> pass;
    };

    struct Route {
        int a, b;

        int level = 0;
        float lenght;
        std::vector<glm::vec2> points; //Not serialized
        std::vector<Plane> planes;
        float lastTakeoffA = 0, lastTakeoffB = 0;

        Route(int a, int b): a(a), b(b) {}
    };

    void renderRoutePath(const Camera& camera, const Route& route);
    void renderRoutePlanes(const Camera& camera, const Route& route, float frameProgress);
    bool routeClicked(const Camera& camera, const Route& route, SDL_Point mousePos);

    float routePrice(const Route& route);

    int getPointGrid(const Camera& camera, glm::vec2 p);

    Coord getIntermediatePoint(Coord c1, Coord c2, float t);
    std::vector<glm::vec2> getPathProjs(const Camera& camera, Coord a, Coord b);
    std::pair<glm::vec2, float> getPointAndAngle(const Route& route, float t);

};
