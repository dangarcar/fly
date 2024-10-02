#include "Airport.hpp"

#include <format>
#include <algorithm>

#include "../game/Camera.hpp"
#include "../engine/Renderer.hpp"
#include "../engine/Utils.h"

#include "../map/Map.hpp"

#include "../Player.hpp"

Coord getIntermediatePoint(Coord c1, Coord c2, float t);
std::vector<glm::vec2> getPathProjs(const Camera& camera, Coord a, Coord b);

std::pair<glm::vec2, float> getPointAndAngle(const Route& route, float t);

std::vector<int> searchPath(int src, const std::vector<std::vector<int>>& adjList);

float getRelativeRadius(int radius, float zoom) {
    return radius * std::clamp(zoom, 2.0f, 12.0f) / 10;
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

    if(auto* mouseupevent = std::get_if<MouseUpEvent>(&event)) {
        if(mouseupevent->button == SDL_BUTTON_LEFT)
            leftDown = false;
    }

    if(auto* clickevent = std::get_if<ClickEvent>(&event)) {
        if(clickevent->button == SDL_BUTTON_LEFT)
            clicked = true;
    }

    if(auto* keyevent = std::get_if<KeyPressedEvent>(&event)) { //FIXME: debug only
        if(keyevent->keycode == SDLK_g) {
            for(auto& v: parentTree) {
                for(auto e: v)
                    writeLog("%03d ", e);
                writeLog("\n");
            }
            writeLog("\n");
        }
    }

    return currentRoute.a != -1;
}

void AirportManager::update(CitySpawner& citySpawner, Camera& camera, Player& player) {
    /*if(routes.size() < 4000) { //FIXME: stress test
        if(airports.size() > 3) {
            int i1 = rand() % airports.size();
            int i2 = rand() % airports.size();

            if(i1 != i2) {
                Coord c1 = cities[i1].coord;
                Coord c2 = cities[i2].coord;

                Route route(i1, i2);
                route.lenght = mtsDistance(c1, c2);
                route.points = getPathProjs(camera, c1, c2);
                this->addRoute(std::move(route), player);
                
                int t = 5;
                while(--t) {
                    this->addPlane(routes.size() -1, player);
                }
            }
        } 
    }*/

    if(auto city = citySpawner.getRandomCity()) {    
        auto value = city.value();   
        addAirport(std::move(value));
        updatePaths();
    }

    for(int i=0; i<agentSpawner.peopleToSpawn(cities.size()); ++i) {
        if(auto agent = agentSpawner.spawn(cities); agent.source != -1) {
            airports[agent.source].waiting.push_back(agent.target);
            airports[agent.source].people++;
            
            /*writeLog("%s (%s) --> %s (%s)\n", //FIXME:
                cities[agent.source].name.c_str(), cities[agent.source].country.c_str(), 
                cities[agent.target].name.c_str(), cities[agent.target].country.c_str()
            );*/
        }
    }

    for(int i=0; i<int(planes.size()); ++i) {
        planes[i].t += planes[i].speed;

        int a = routes[planes[i].routeIndex].a;
        int b = routes[planes[i].routeIndex].b;

        if(planes[i].t <= 0.0f || planes[i].t >= 1.0f) {
            planes[i].t = std::clamp(planes[i].t, 0.0f, 1.0f);
            if(planes[i].t >= 1.0f) //Change the variables so it works the other way
                std::swap(a, b);
            
            planes[i].speed = -planes[i].speed;
            
            landPlane(player, planes[i], a, b);
        } 
    }

    //Timer t;
    int clickedAirport = -1;
    for(size_t i=0; i<airports.size(); ++i) {
        auto pos = camera.projToScreen(cities[i].proj);
        auto dist = SDL_distance({int(pos.x), int(pos.y)}, mousePos);
            
        if(dist < getRelativeRadius(airports[i].radius, camera.getZoom())) {
            clickedAirport = i;
            break;
        }
    }

    int clickedPlane = -1;
    for(size_t i=0; i<planes.size(); ++i) {
        auto& plane = planes[i];
        auto [proj, angle] = getPointAndAngle(routes[plane.routeIndex], plane.t);
        auto p = camera.projToScreen(proj);
    }
    //writeLog("%f\n", t.elapsedMillis());

    if(clicked) { //FIXME: debug purposes
        if(clickedAirport != -1) {
            //writeLog("%s -> %d\n", cities[clickedAirport].name.c_str(), clickedAirport);

            std::vector<std::vector<int>> paths(airports.size());

            for(int i=0; i<int(paths.size()); ++i) {
                int a = clickedAirport;
                paths[i].push_back(a);
                while(a != -1 && a != i) {
                    a = getNextAirport(a, i);
                    paths[i].push_back(a);
                }
            }

            writeLog("\n");
            for(auto p: paths) {
                for(auto e: p) {
                    writeLog("%s -> ", cities[e].name.c_str());
                }
                writeLog("\n");
            }
            writeLog("\n");
        }
        clicked = false;
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

        if(clickedAirport != -1) {
            if(currentRoute.a == -1)
                currentRoute.a = clickedAirport;
            else if(currentRoute.a != clickedAirport)
                currentRoute.b = clickedAirport;
        } else {
            currentRoute.b = -1;
        }
    } else {
        if(currentRoute.a != -1 && currentRoute.b != -1)
            this->addRoute(std::move(currentRoute), player);
        currentRoute = Route(-1, -1);
    }

    if(currentRoute.a != -1) {
        Coord c1 = cities[currentRoute.a].coord;
        Coord c2 = currentRoute.b==-1? camera.screenToCoords(mousePos) : cities[currentRoute.b].coord;
        
        currentRoute.lenght = mtsDistance(c1, c2);
        currentRoute.points = getPathProjs(camera, c1, c2);
        currentPrice = currentRoute.lenght * player.getDifficulty() * PRICE_PER_METER_ROUTE;
    }
}

void AirportManager::render(const Camera& camera, float frameProgress) {
    for(const auto& r: routes) {
        renderRoute(camera, r);
    }

    for(int i=0; i<int(airports.size()); ++i) {
        renderAirport(camera, i);
    }

    for(const auto& pl: planes) {
        renderPlane(camera, pl, frameProgress);
    }

    if(currentRoute.a != -1) {
        Coord c1 = cities[currentRoute.a].coord;
        Coord c2 = currentRoute.b==-1? camera.screenToCoords(mousePos) : cities[currentRoute.b].coord;
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

    auto t = std::format("\nZoom level: {}\n\n\n{}", camera.getZoom(), routes.size());
    camera.renderText(t, 0, 0, 32, FC_ALIGN_LEFT, SDL_WHITE);
}

void AirportManager::renderAirport(const Camera& camera, int airportIndex) const {
    auto& air = airports[airportIndex];

    auto& circle = camera.getTextureManager().getTexture("CIRCLE");
    auto& country = camera.getTextureManager().getTexture(cities[airportIndex].country);

    float fillPercentage = std::min(1.0f, float(air.people) / AIRPORT_CAPACITY_PER_LEVEL[air.level]);
    auto color = FULL_GRADIENT.getColor(fillPercentage);
    
    auto pos = camera.projToScreen(cities[airportIndex].proj);
    if(pos.x < 0 || pos.y < 0 || pos.x > camera.getWidth() || pos.y > camera.getHeight())
        return;

    auto rad = getRelativeRadius(air.radius, camera.getZoom());
    SDL_Rect clip = {int(pos.x - rad), int(pos.y - rad), int(2.0*rad), int(2.0*rad)};

    SDL_Rect perimeter;
    if(cities[airportIndex].capital)
        perimeter = SDL_Rect { clip.x - 3, clip.y - 3, clip.w + 6, clip.h + 6 };
    else
        perimeter = SDL_Rect { clip.x - 2, clip.y - 2, clip.w + 4, clip.h + 4 };
    
    circle.setColorMod(SDL_BLACK);
    circle.render(*camera.getSDL(), perimeter.x, perimeter.y, &perimeter);
    country.render(*camera.getSDL(), clip.x, clip.y, &clip);

    if((camera.getZoom() > 4 && air.radius >= 20)
    || (camera.getZoom() > 6 && air.radius >= 16)
    || camera.getZoom() > 8) {
        //DRAW NAME
        camera.renderText(cities[airportIndex].name, clip.x + rad, clip.y - rad*1.3, rad*1.3, FC_ALIGN_CENTER, color);

        //DRAW BAR
        auto bar = SDL_Rect { clip.x, clip.y + clip.h + 4, clip.w, clip.h/5 };
        SDL_SetRenderDrawColor(camera.getSDL(), 0, 0, 0, 255);
        SDL_RenderFillRect(camera.getSDL(), &bar);

        bar = SDL_Rect { bar.x + 1, bar.y + 1, int((bar.w - 2) * fillPercentage), bar.h - 2 };
        SDL_SetRenderDrawColor(camera.getSDL(), color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(camera.getSDL(), &bar);
    }
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

void AirportManager::renderPlane(const Camera& camera, const Plane& plane, float frameProgress) const {
    const auto& route = routes[plane.routeIndex];
    auto [proj, angle] = getPointAndAngle(route, std::clamp(plane.t + plane.speed*frameProgress, 0.0f, 1.0f));
    auto p = camera.projToScreen(proj);
    auto& t = camera.getTextureManager().getTexture("PLANE");
    auto scale = std::clamp(camera.getZoom(), 2.0f, 12.0f) * 0.009f;

    auto distA = glm::distance(proj, cities[route.a].proj) * camera.getZoom();
    auto distB = glm::distance(proj, cities[route.b].proj) * camera.getZoom();
    
    auto fillPercentage = std::min(1.0f, float(plane.people) / PLANE_CAPACITY_PER_LEVEL[plane.level]);
    assert(fillPercentage >= 0.0f);

    if(distA > scale * t.getWidth()/2 && distB > scale * t.getWidth()/2) {
        t.setColorMod(FULL_GRADIENT.getColor(fillPercentage));
        t.renderCenter(*camera.getSDL(), p.x, p.y, scale, angle + 180.0f * (plane.speed < 0));
    }
}

void AirportManager::addRoute(Route&& r, Player& player) {
    if(std::find(networkAdjList[r.a].begin(), networkAdjList[r.a].end(), r.b) != networkAdjList[r.a].end())
        return;

    auto& route = routes.emplace_back(r);
    
    player.spend(currentPrice);
    currentPrice = 0;
    
    airports[route.a].routeIndexes.emplace_back(routes.size() -1);
    airports[route.b].routeIndexes.emplace_back(routes.size() -1);

    networkAdjList[route.a].emplace_back(route.b);
    networkAdjList[route.b].emplace_back(route.a);

    updatePaths();
    addPlane(routes.size() - 1, player);
}

void AirportManager::addPlane(int routeIndex, Player& player) {
    Plane p;
    p.t = 0.0f;
    p.people = 0;
    p.speed = MTS_PER_TICK_PER_LEVEL[0] / routes[routeIndex].lenght;
    p.routeIndex = routeIndex;

    auto& plane = planes.emplace_back(p);
    landPlane(player, plane, routes[plane.routeIndex].a, routes[plane.routeIndex].b);
}

void AirportManager::addAirport(City&& c) {
    auto& city = cities.emplace_back(c);

    AirportData data;
    if(city.capital) data.radius = 20;
    else if(city.population > 1e6) data.radius = 16;
    else data.radius = 12;

    airports.emplace_back(std::move(data));
}

void AirportManager::updatePaths() {
    parentTree.resize(airports.size());
    networkAdjList.resize(airports.size());
    
    #pragma omp parallel for
    for(int i=0; i<int(airports.size()); ++i) {
        parentTree[i] = searchPath(i, networkAdjList);
    }
}

void AirportManager::landPlane(Player& player, Plane &plane, int a, int b) {
    // GET OFF THE PLANE
    player.earn(PRICE_PER_FLIGHT * plane.people);
    auto &wait = airports[a].waiting;
    for(int i=0; i<int(plane.people); ++i) {
        if(plane.pass[i] != a) {
            wait.emplace_back(plane.pass[i]);
            airports[a].people++;
        }
    }

    // GET ON THE PLANE
    plane.pass.clear();
    int capacity = PLANE_CAPACITY_PER_LEVEL[plane.level];
    plane.pass.resize(capacity);
    plane.people = 0;

    auto it = wait.begin();
    while(it != wait.end()) {
        if(plane.people == capacity)
            break;
        assert(plane.people < capacity);

        int p = *it;
        if(getNextAirport(a, p) == b) {
            plane.pass[plane.people] = p;
            plane.people++;
            it = wait.erase(it);
            if(it != wait.end())
                airports[a].people--;
        } else {
            ++it;
        }
    }
}

std::vector<int> searchPath(int src, const std::vector<std::vector<int>>& adjList) {
    int n = adjList.size();
    assert(src < n);

    std::vector<int> parent(n, -1);
    bool visited[n];
    memset(visited, false, n);
    std::queue<int> q;

    parent[src] = src;
    visited[src] = true;
    q.push(src);

    while(!q.empty()) {
        auto u = q.front();
        q.pop();

        for(auto v: adjList[u]) {
            if(!visited[v]) {
                q.push(v);
                visited[v] = true;
                parent[v] = u;
            }
        }
    }

    return parent;
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
    #pragma omp parallel for
    for(int i=0; i<=n; ++i) {
        auto c = getIntermediatePoint(a, b, float(i)/n);
        path[i] = camera.coordsToProj(c);
    }

    return path;
}

std::pair<glm::vec2, float> getPointAndAngle(const Route& route, float t) {
    float fi = t * (route.points.size() - 1);
    int i1 = std::floor(fi), i2 = std::ceil(fi);
    
    auto v = glm::normalize(route.points[i1] - route.points[i2]);
    auto angle = glm::degrees( std::acos(glm::dot(glm::vec2(0.0f, 1.0f), v)) );
    if(route.points[i1].x > route.points[i2].x)
        angle = 360 - angle;
    return std::make_pair(glm::mix(route.points[i1], route.points[i2], fi - i1), angle);
}
