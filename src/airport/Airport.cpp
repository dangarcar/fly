#include "Airport.hpp"

#include <algorithm>

#include "../game/Camera.hpp"

float air::getRelativeRadius(float radius, float zoom) {
    return radius * std::clamp(zoom, 2.0f, 12.0f);
}

void air::renderAirport(const Camera& camera, const AirportData& airport, const City& city) {
    auto& circle = camera.getTextureManager().getTexture("CIRCLE");
    auto& country = camera.getTextureManager().getTexture(city.country);

    float fillPercentage = std::min(1.0f, float(airport.waiting.size()) / AIRPORT_CAPACITY_PER_LEVEL[airport.level]);
    auto color = FULL_GRADIENT.getColor(fillPercentage);

    auto pos = camera.projToScreen(city.proj);
    if(pos.x < 0 || pos.y < 0 || pos.x > camera.getWidth() || pos.y > camera.getHeight())
        return;

    auto rad = getRelativeRadius(airport.radius, camera.getZoom());
    SDL_Rect clip = {int(pos.x - rad), int(pos.y - rad), int(2.0*rad), int(2.0*rad)};

    SDL_Rect perimeter;
    if(city.capital)
        perimeter = SDL_Rect { clip.x - 3, clip.y - 3, clip.w + 6, clip.h + 6 };
    else
        perimeter = SDL_Rect { clip.x - 2, clip.y - 2, clip.w + 4, clip.h + 4 };

    circle.setColorMod(SDL_BLACK);
    circle.render(camera.getSDL(), perimeter.x, perimeter.y, &perimeter);
    country.render(camera.getSDL(), clip.x, clip.y, &clip);

    if((camera.getZoom() > 4 && airport.radius >= 20)
    || (camera.getZoom() > 6 && airport.radius >= 16)
    || camera.getZoom() > 8) {
        //DRAW NAME
        camera.renderText(city.name, clip.x + rad, clip.y - rad*1.3, rad*1.3, FC_ALIGN_CENTER, color);

        //DRAW BAR
        auto bar = SDL_Rect { clip.x, clip.y + clip.h + 4, clip.w, clip.h/5 };
        SDL_SetRenderDrawColor(&camera.getSDL(), 0, 0, 0, 255);
        SDL_RenderFillRect(&camera.getSDL(), &bar);

        bar = SDL_Rect { bar.x + 1, bar.y + 1, int((bar.w - 2) * fillPercentage), bar.h - 2 };
        SDL_SetRenderDrawColor(&camera.getSDL(), color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(&camera.getSDL(), &bar);
    }
}