#include "Airport.hpp"

#include <format>

#include "game/Camera.hpp"
#include "engine/Renderer.hpp"

void Airport::render(const Camera& camera) const {
    auto& t = camera.getTextureManager().getTexture("CITY_CIRCLE");

    auto pos = camera.projToScreen(city.proj);
    auto rad = radius * std::clamp(camera.getZoom(), 2.0f, 12.0f) / 10;
    SDL_Rect clip = {0, 0, int(2.0*rad), int(2.0*rad)};
    pos.x -= rad; pos.y -= rad;

    auto color = gradient.getColor(0.5);

    t.setColorMod(color);
    t.render(*camera.getSDL(), pos.x, pos.y, &clip);

    if((camera.getZoom() > 4 && radius >= 20)
    || (camera.getZoom() > 8 && radius >= 16)
    || camera.getZoom() > 12)
        camera.renderText(city.name, pos.x + rad, pos.y - rad*1.3, rad*1.3, FC_ALIGN_CENTER, color);
}

void Airport::update() {
    //TODO: 
}

void AirportManager::registerEvents(Event::EventManager& manager) {
    /*if(auto clickevent = std::get_if<ClickEvent>(&event)) {
        clickevent->clickPoint
    }*/
    //TODO: 
}

void AirportManager::update(CitySpawner& citySpawner) {
    if(auto city = citySpawner.getRandomCity()) {
        airports.push_back(city.value());
    }

    for(auto& air: airports) {
        air.update();
    }
}

void AirportManager::render(const Camera& camera) {
    for(auto& air: airports) {
        air.render(camera);
    }

    auto t = std::format("Airports: {}\nZoom level: {}", airports.size(), camera.getZoom());
    camera.renderText(t, 0, 0, 32, FC_ALIGN_LEFT, SDL_WHITE);
}
