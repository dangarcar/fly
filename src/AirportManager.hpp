#pragma once

#include "Map.hpp"
#include "Airport.hpp"

class AirportManager: GameObject {
public:
    AirportManager(Camera& cam): camera(cam), map(cam) {}

    void handleEvents(const Event& event) override;
    void update() override;
    void render(const Renderer& renderer) override;
    void load(const Renderer& renderer) override { map.load(renderer); }

private:
    std::vector<Airport> airports;
    Map map;

    const Camera& camera;
};

void AirportManager::handleEvents(const Event& event) {
    map.handleEvents(event);
}

void AirportManager::update() {
    map.update();
    
    if(auto city = map.getRandomCity()) {
        airports.push_back(city.value());
    }

    for(auto& air: airports) {
        air.update();
    }
}

void AirportManager::render(const Renderer& renderer) {
    map.render(renderer);

    for(auto& air: airports) {
        air.render(renderer, camera);
    }

    auto t = std::format("Airports: {}\nZoom level: {}", airports.size(), camera.getZoom());
    renderer.renderText(t, 0, 0, 32, FC_ALIGN_LEFT, SDL_WHITE);
}