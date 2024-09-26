#include "Airport.hpp"

#include <format>

#include "game/Camera.hpp"
#include "engine/Renderer.hpp"
#include "engine/Utils.h"

#include "Player.hpp"

static float fillPercentage = 0; //FIXME: it's to debug only

Coord getIntermediatePoint(Coord c1, Coord c2, float t);
std::vector<glm::vec2> getPathProjs(const Camera& camera, Coord a, Coord b);

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
    fillPercentage -= 0.000001;
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

void AirportManager::update(CitySpawner& citySpawner, Camera& camera, Player& player) {
    if(auto city = citySpawner.getRandomCity()) {        
        airports.push_back(city.value());
    }

    for(auto& air: airports) {
        air.update();
    }

    for(auto& route: routes) {
        for(auto& p: route.planes) {
            p.t += p.speed;

            if(p.t <= 0 || p.t >= 1)
                p.speed = -p.speed;
        }
    }

    if(leftDown) {
        if(mousePos.x <= 0)
            camera.move(glm::vec2(10 / camera.getZoom(), 0.0f));
        else if(mousePos.x >= camera.getWidth()-1)
            camera.move(glm::vec2(-10 / camera.getZoom(), 0.0f));

        if(mousePos.y <= 0)
            camera.move(glm::vec2(0.0f, 10 / camera.getZoom()));
        else if(mousePos.y >= camera.getHeight()-1)
            camera.move(glm::vec2(0.0f, -10 / camera.getZoom()));

        for(size_t i=0; i<airports.size(); ++i) {
            if (i == static_cast<size_t>(currentRoute.a))
                continue;

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
            this->addRoute(std::move(currentRoute), player);
        currentRoute = {-1, -1};
    }

    if(currentRoute.a != -1) {
        Coord c1 = airports[currentRoute.a].getCity().coord;
        Coord c2 = currentRoute.b==-1? camera.screenToCoords(mousePos) : airports[currentRoute.b].getCity().coord;
        
        currentRoute.lenght = mtsDistance(c1, c2);
        currentRoute.points = getPathProjs(camera, c1, c2);
        currentPrice = currentRoute.lenght * player.getDifficulty() * PRICE_PER_METER_ROUTE;
    }
}

void AirportManager::render(const Camera& camera, float frameProgress) {
    for(auto& r: routes) {
        renderRoute(camera, r);
    }

    for(auto& air: airports) {
        air.render(camera);
    }

    for(auto& r: routes) {
        for(auto& pl: r.planes) {
            auto [proj, angle] = r.getPointAndAngle(pl.t + pl.speed*frameProgress);
            auto p = camera.projToScreen(proj);
            auto& t = camera.getTextureManager().getTexture("PLANE");
            auto scale = std::clamp(camera.getZoom(), 2.0f, 15.0f) * 0.01f;

            auto distA = glm::distance(proj, airports[r.a].getCity().proj) * camera.getZoom();
            auto distB = glm::distance(proj, airports[r.b].getCity().proj) * camera.getZoom();
            if(distA > scale * t.getWidth()/2 && distB > scale * t.getWidth()/2) {
                t.renderCenter(*camera.getSDL(), p.x, p.y, scale, angle + 180.0f * (pl.speed < 0));
            }
        }
    }

    if(currentRoute.a != -1) {
        Coord c1 = airports[currentRoute.a].getCity().coord;
        Coord c2 = currentRoute.b==-1? camera.screenToCoords(mousePos) : airports[currentRoute.b].getCity().coord;
        SDL_SetRenderDrawColor(camera.getSDL(), 0xff, 0xff, 0xff, 0xff);
        int n = float(mtsDistance(c1, c2)) / EARTH_RADIUS * std::clamp(camera.getZoom(), 2.0f, 20.0f) * 20;
        n += n % 2 + 1;

        Coord lastCoord;
        for(int i=0; i<=n; ++i) {
            auto c = getIntermediatePoint(c1, c2, float(i)/n);
            if(i%2 == 1) {
                auto lastPoint = camera.coordsToScreen(lastCoord);
                auto p = camera.coordsToScreen(c);

                if(std::abs(lastPoint.x - p.x) < camera.getWidth()/2)
                    SDL_RenderDrawLine(camera.getSDL(), int(p.x), int(p.y), int(lastPoint.x), int(lastPoint.y));
            }
            lastCoord = c;
        }

        camera.renderText(std::to_string(currentPrice), mousePos.x, mousePos.y - 36, 32, FC_ALIGN_CENTER, SDL_WHITE);
    }

    auto t = std::format("\nZoom level: {}\n\n\n", camera.getZoom());
    camera.renderText(t, 0, 0, 32, FC_ALIGN_LEFT, SDL_WHITE);
}

void AirportManager::renderRoute(const Camera& camera, const Route& route) const {    
    float dist = route.lenght / EARTH_RADIUS;
    int n = dist * std::clamp(camera.getZoom(), 2.0f, 20.0f) * 20;
    n += n % 2 + 1;

    SDL_SetRenderDrawColor(camera.getSDL(), 0xff, 0xff, 0xff, 0xff);
    
    glm::vec2 lastProj;
    for(int i=0; i<=n; ++i) {
        auto t = float(i) / n;
        auto proj = route.points[ static_cast<size_t>(t * (route.points.size()-1)) ];

        if(i % 2 == 1) {
            auto lastPoint = camera.projToScreen(lastProj);
            auto p = camera.projToScreen(proj);

            if(std::abs(lastPoint.x - p.x) < camera.getWidth()/2)
                SDL_RenderDrawLine(camera.getSDL(), int(p.x), int(p.y), int(lastPoint.x), int(lastPoint.y));
        }
        
        lastProj = proj;
    }
}

void AirportManager::addRoute(Route&& r, Player& player) {
    auto& route = routes.emplace_back(r);
    
    player.spend(currentPrice);
    currentPrice = 0;
    
    airports[route.a].routeIndexes.push_back(routes.size() -1);
    airports[route.b].routeIndexes.push_back(routes.size() -1);

    airports[route.a].connectedAirports.push_back(route.b);
    airports[route.b].connectedAirports.push_back(route.a);

    route.planes.push_back(Plane { .t = 0.0, .speed = MTS_PER_TICK / route.lenght, .capacity = 20});
}

Coord getIntermediatePoint(Coord c1, Coord c2, float t) {    
    auto lat1 = glm::radians(c1.lat), lon1 = glm::radians(c1.lon);
    auto lat2 = glm::radians(c2.lat), lon2 = glm::radians(c2.lon);

    auto sinlat = std::sin((lat1-lat2) / 2);
    auto sinlon = std::sin((lon1-lon2) / 2);
    auto tmp = sinlat*sinlat + std::cos(lat1) * std::cos(lat2) * sinlon*sinlon;
    auto d = 2.0 * std::asin(std::sqrt(tmp));

    auto A = std::sin((1-t) * d) / std::sin(d);
    auto B = std::sin(t * d) / std::sin(d);
    auto x = A * std::cos(lat1) * std::cos(lon1) + B * std::cos(lat2) * std::cos(lon2);
    auto y = A * std::cos(lat1) * std::sin(lon1) + B * std::cos(lat2) * std::sin(lon2);
    auto z = A * std::sin(lat1) + B * std::sin(lat2);
    
    Coord result;
    result.lat = glm::degrees(std::atan2(z, std::sqrt(x*x + y*y)));
    result.lon = glm::degrees(std::atan2(y, x));
    return result;
}

std::vector<glm::vec2> getPathProjs(const Camera& camera, Coord a, Coord b) {
    int n = mtsDistance(a, b) * Camera::MAX_ZOOM * 100 / EARTH_RADIUS;

    std::vector<glm::vec2> path(n+1);
    for(int i=0; i<=n; ++i) {
        auto c = getIntermediatePoint(a, b, float(i)/n);
        path[i] = camera.coordsToProj(c);
    }

    return path;
}
