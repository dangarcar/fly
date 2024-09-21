#include "Airport.hpp"

#include <format>

#include "game/Camera.hpp"
#include "engine/Renderer.hpp"
#include "engine/Timer.h"

static float fillPercentage = 0; //FIXME: it's to debug only

void Airport::render(const Camera& camera) const {    
    auto& circle = camera.getTextureManager().getTexture("CIRCLE");
    auto& country = camera.getTextureManager().getTexture(this->city.country);
    auto color = gradient.getColor(fillPercentage);
    
    auto pos = camera.projToScreen(city.proj);
    auto rad = radius * std::clamp(camera.getZoom(), 2.0f, 12.0f) / 10;
    SDL_Rect clip = {int(pos.x - rad), int(pos.y - rad), int(2.0*rad), int(2.0*rad)};

    SDL_Rect perimeter;
    if(city.capital)
        perimeter = SDL_Rect { clip.x - 3, clip.y - 3, clip.w + 6, clip.h + 6 };
    else
        perimeter = SDL_Rect { clip.x - 2, clip.y - 2, clip.w + 4, clip.h + 4 };
    
    circle.setColorMod(SDL_BLACK);
    circle.render(*camera.getSDL(), perimeter.x, perimeter.y, &perimeter);    
    country.render(*camera.getSDL(), clip.x, clip.y, &clip);

    if((camera.getZoom() > 4 && radius >= 20)
    || (camera.getZoom() > 8 && radius >= 16)
    || camera.getZoom() > 12) {
        //DRAW NAME
        camera.renderText(city.name, clip.x + rad, clip.y - rad*1.3, rad*1.3, FC_ALIGN_CENTER, color);

        //DRAW BAR
        auto bar = SDL_Rect { clip.x, clip.y + clip.h + 4, clip.w, clip.h/5 };
        SDL_SetRenderDrawColor(camera.getSDL(), 0, 0, 0, 255);
        SDL_RenderFillRect(camera.getSDL(), &bar);

        bar = SDL_Rect { bar.x + 1, bar.y + 1, int((bar.w - 2) * fillPercentage), bar.h - 2 };
        SDL_SetRenderDrawColor(camera.getSDL(), color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(camera.getSDL(), &bar);
    }
}

void Airport::update() {
    fillPercentage -= 0.001;
    if(fillPercentage < 0)
        fillPercentage = 1;
}

void AirportManager::handleInput(const InputEvent& event) {
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
    Timer timer;
    for(auto& air: airports) {
        air.render(camera);
    }

    auto t = std::format("Airport render: {}ms\nZoom level: {}", timer.elapsedMillis(), camera.getZoom());
    camera.renderText(t, 0, 0, 32, FC_ALIGN_LEFT, SDL_WHITE);
}
