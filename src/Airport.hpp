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

    void update();
    void render(const Camera& camera) const;
};

class AirportManager {
public:
    void handleInput(const InputEvent& event);
    void update(CitySpawner& citySpawner);
    void render(const Camera& camera);

private:
    std::vector<Airport> airports;
};
