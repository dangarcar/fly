#pragma once

#include "CitySpawner.hpp"
#include "Airport.hpp"

class AirportManager {
public:
    AirportManager() = default;

    void handleEvents(const SystemEvent& event);
    void update(CitySpawner& citySpawner);
    void render(const Renderer& renderer, const PlayerCamera& camera);
    void load(const Renderer& renderer) {}

private:
    std::vector<Airport> airports;

};

void AirportManager::handleEvents(const SystemEvent& event) {
    /*if(auto clickevent = std::get_if<ClickEvent>(&event)) {
        clickevent->clickPoint
    }*/
    //NOTHING HERE!!
}

void AirportManager::update(CitySpawner& citySpawner) {
    if(auto city = citySpawner.getRandomCity()) {
        airports.push_back(city.value());
    }

    for(auto& air: airports) {
        air.update();
    }
}

void AirportManager::render(const Renderer& renderer, const PlayerCamera& camera) {
    for(auto& air: airports) {
        air.render(renderer, camera);
    }

    auto t = std::format("Airports: {}\nZoom level: {}", airports.size(), camera.getZoom());
    renderer.renderText(t, 0, 0, 32, FC_ALIGN_LEFT, SDL_WHITE);
}