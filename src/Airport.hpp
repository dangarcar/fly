#pragma once

#include "map/Map.hpp"
#include "engine/Gradient.h"
#include "engine/InputEvent.h"

class Airport {
private:
    static constexpr Gradient gradient = {{0x55, 0xBF, 0x40, SDL_ALPHA_OPAQUE}, {0x7D, 0x40, 0xBF, SDL_ALPHA_OPAQUE}};

    City city;
    int level;
    int radius;

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
};

class AirportManager {
public:
    bool handleInput(const InputEvent& event);
    void update(CitySpawner& citySpawner, Camera& camera);
    void render(const Camera& camera);

    void addRoute(const Route& route);

private:
    void renderRoute(const Camera& camera, const Route& route) const;

private:
    std::vector<Airport> airports;
    std::vector<Route> routes;

    SDL_Point mousePos;
    bool leftDown;
    Route currentRoute = {-1, -1};
};
