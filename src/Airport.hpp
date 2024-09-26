#pragma once

#include "map/Map.hpp"
#include "engine/Gradient.h"
#include "engine/InputEvent.h"

constexpr float PRICE_PER_METER_ROUTE = 0.0003;
constexpr double MTS_PER_TICK = 500;

class AirportManager;

struct Plane {
    double t;
    double speed;

    int capacity;
};

class Airport {
    friend class AirportManager;

private:
    static constexpr Gradient gradient = {{0x55, 0xBF, 0x40, SDL_ALPHA_OPAQUE}, {0x7D, 0x40, 0xBF, SDL_ALPHA_OPAQUE}};

    City city;
    int level;
    int radius;

    std::vector<size_t> routeIndexes, connectedAirports;

public:
    Airport(City city): city(city), level(0) {
        if(city.capital) radius = 20;
        else if(city.population > 1e6) radius = 16;
        else radius = 12;
    }

    const City& getCity() const { return city; }
    float getRelativeRadius(float zoom) const { return radius * std::clamp(zoom, 2.0f, 15.0f) / 10; }

    void update();
    void render(const Camera& camera) const;
};

struct Route {
    int a, b;

    float lenght;
    std::vector<glm::vec2> points;
    std::vector<Plane> planes;

    Route(int a, int b): a(a), b(b) {}
    
    std::pair<glm::vec2, float> getPointAndAngle(float t) const {
        float fi = t * (points.size() - 1);
        int i1 = std::floor(fi), i2 = std::ceil(fi);
        
        auto v = glm::normalize(points[i1] - points[i2]);
        auto angle = glm::degrees( std::acos(glm::dot(glm::vec2(0.0f, 1.0f), v)) );

        if(points[i1].x > points[i2].x)
            angle = 360 - angle;

        return std::make_pair(glm::mix(points[i1], points[i2], fi - i1), angle);
    }
};

class AirportManager {
public:
    bool handleInput(const InputEvent& event);
    void update(CitySpawner& citySpawner, Camera& camera, Player& player);
    void render(const Camera& camera, float frameProgress);

private:
    void addRoute(Route&& route, Player& player);
    void renderRoute(const Camera& camera, const Route& route) const;

private:
    std::vector<Airport> airports;
    std::vector<Route> routes;

    SDL_Point mousePos;
    bool leftDown;
    Route currentRoute {-1, -1};
    long currentPrice = 0;
};
