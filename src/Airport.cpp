#include "Airport.hpp"

#include <format>

#include "game/Camera.hpp"
#include "engine/Renderer.hpp"
#include "engine/Utils.h"

static float fillPercentage = 0; //FIXME: it's to debug only

void Airport::render(const Camera& camera) const {    
    auto& circle = camera.getTextureManager().getTexture("CIRCLE");
    auto& country = camera.getTextureManager().getTexture(this->city.country);
    auto color = gradient.getColor(fillPercentage);
    
    auto pos = camera.projToScreen(city.proj);
    if(pos.x < 0 || pos.y < 0 || pos.x > camera.getWidth() || pos.y > camera.getHeight())
        return;

    auto rad = getRelativeRadius(camera.getZoom());
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
    || (camera.getZoom() > 6 && radius >= 16)
    || camera.getZoom() > 8) {
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
    fillPercentage -= 0.00001;
    if(fillPercentage < 0)
        fillPercentage = 1;
}

bool AirportManager::handleInput(const InputEvent& event) {
    if(auto* moveevent = std::get_if<MouseMoveEvent>(&event)) {
        mousePos = moveevent->newPos;

        if(!leftDown && moveevent->leftDown) {
            leftDown = true;
            return true;
        }

        leftDown = moveevent->leftDown;
    }

    if([[maybe_unused]] auto* resizedevent = std::get_if<MouseUpEvent>(&event)) {
        leftDown = false;
    }

    return currentRoute.a != -1;
}

void AirportManager::update(CitySpawner& citySpawner, Camera& camera) {
    if(auto city = citySpawner.getRandomCity()) {        
        airports.push_back(city.value());
    }

    for(auto& air: airports) {
        air.update();
    }

    if(leftDown) {
        if(mousePos.x <= 0)
            camera.move(glm::vec2(10 / camera.getZoom(), 0.0f));
        else if(mousePos.x >= camera.getWidth())
            camera.move(glm::vec2(-10 / camera.getZoom(), 0.0f));

        if(mousePos.y <= 0)
            camera.move(glm::vec2(0.0f, 10 / camera.getZoom()));
        else if(mousePos.y >= camera.getHeight())
            camera.move(glm::vec2(0.0f, -10 / camera.getZoom()));

        for(size_t i=0; i<airports.size(); ++i) {
            if (i == static_cast<size_t>(currentRoute.a)) continue;

            auto pos = camera.projToScreen(airports[i].getCity().proj);
            auto dist = SDL_distance({int(pos.x), int(pos.y)}, mousePos);
                
            if(dist < airports[i].getRelativeRadius(camera.getZoom())) {
                if(currentRoute.a == -1)
                    currentRoute.a = i;
                    
                currentRoute.b = i;
                break;
            }

            currentRoute.b = -1;
        }
    } else {
        if(currentRoute.a != -1 && currentRoute.b != -1)
            this->addRoute(currentRoute);
        currentRoute = {-1, -1};
    }
}

void AirportManager::render(const Camera& camera) {
    if(currentRoute.a != -1) {
        auto coordA = camera.projToCoords(airports[currentRoute.a].getCity().proj);
        auto coordB = camera.screenToCoords(mousePos);
        auto dist = mtsDistance(coordA, coordB);
        writeLog("%d\n", dist / 1000);

        renderRoute(camera, currentRoute);
    }

    for(auto& r: routes) {
        renderRoute(camera, r);
    }

    for(auto& air: airports) {
        air.render(camera);
    }

    auto t = std::format("\nZoom level: {}", camera.getZoom());
    camera.renderText(t, 0, 0, 32, FC_ALIGN_LEFT, SDL_WHITE);
}

void AirportManager::renderRoute(const Camera& camera, const Route& route) const {
    auto p1 = airports[route.a].getCity().proj;
    auto c1 = camera.projToScreen(p1);

    SDL_Point c2;
    if(route.b == -1) {
        c2 = mousePos;
    } else {
        auto p = camera.projToScreen(airports[route.b].getCity().proj);
        c2 = {int(p.x), int(p.y)};
    }
    
    SDL_RenderDrawLine(camera.getSDL(), c1.x, c1.y, c2.x, c2.y);
}

void AirportManager::addRoute(const Route& route) {
    routes.push_back(currentRoute);
}
