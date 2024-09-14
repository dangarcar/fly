#pragma once

#include "Map.hpp"
#include "engine/Gradient.hpp"

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
    void render(const Renderer& renderer, const Camera& camera) const;
};

void Airport::render(const Renderer& renderer, const Camera& camera) const {
    auto& t = renderer.getTextureManager().getTexture("CITY_CIRCLE");

    auto pos = camera.projToScreen(city.proj);
    auto rad = radius * std::clamp(camera.getZoom(), 2.0f, 12.0f) / 10;
    SDL_Rect clip = {0, 0, (int)2*rad, (int)2*rad};
    pos.x -= rad; pos.y -= rad;

    auto color = gradient.getColor(0.5);

    t.setColorMod(color);
    t.render(*renderer.getSDL(), pos.x, pos.y, &clip);

    if((camera.getZoom() > 4 && radius >= 20)
    || (camera.getZoom() > 8 && radius >= 16)
    || camera.getZoom() > 12)
        renderer.renderText(city.name, pos.x + rad, pos.y - rad*1.3, rad*1.3, FC_ALIGN_CENTER, color);
}

void Airport::update() {
    //TODO: 
}
