#include "AirportManager.hpp"

#include <format>
#include <algorithm>

#include "../game/Camera.hpp"
#include "../engine/Renderer.hpp"
#include "../engine/Utils.h"

#include "../map/Map.hpp"

#include "../Player.hpp"

#include "../ui/UIManager.hpp"
#include "../ui/dialogs/AirportDialog.hpp"
#include "../ui/dialogs/RouteDialog.hpp"

std::vector<int> searchPath(int src, const std::vector<std::vector<int>>& adjList);

void air::AirportManager::deleteRoute(int routeIndex) {
    routes.erase(routes.begin() + routeIndex);
}

bool air::AirportManager::handleInput(const InputEvent& event, Player& player, UIManager& uiManager) {
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
        if(clickevent->button == SDL_BUTTON_LEFT) {   
            if(clickedAirport != -1) {
                uiManager.addDialog<AirportDialog>(airports[clickedAirport], cities[clickedAirport], player, cities);
                return true;
            } else if(clickedRoute != -1) {
                uiManager.addDialog<RouteDialog>(clickedRoute, routes[clickedRoute], player, *this, cities);
                return true;
            }
        }
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

    return currentRoute.route.a != -1;
}

void air::AirportManager::update(CitySpawner& citySpawner, Camera& camera, Player& player, UIManager& uiManager) {
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
        }
    }

    for(int i=0; i<int(routes.size()); ++i) {
        for(int j=0; j<int(routes[i].planes.size()); ++j) {
            auto& p = routes[i].planes[j];
            p.t += p.speed;

            if(p.speed == 0) {
                int k = (j+1) % int(routes[i].planes.size());
                auto newSpeed = (p.t<=0.0f? 1:-1) * MTS_PER_TICK_PER_LEVEL[routes[i].level] / routes[i].lenght;
                auto distance = 1.0f / (routes[i].planes.size()-1) - std::abs(newSpeed);

                if(routes[i].planes[k].t == p.t) {
                    p.speed = newSpeed;
                } 
                else if(std::signbit(newSpeed) == std::signbit(routes[i].planes[k].speed)) {
                    if(std::abs(routes[i].planes[k].t - p.t) >= distance)
                        p.speed = newSpeed;
                } else {
                    if(std::abs(routes[i].planes[k].t - p.t) <=  distance)
                        p.speed = newSpeed;
                }
            } 
            else if(p.t <= 0.0f || p.t >= 1.0f) {
                landPlane(&player, p, routes[i], p.t >= 1.0f);
                
                p.t = std::clamp(p.t, 0.0f, 1.0f);
                p.speed = 0;        
            }
        }
    }

    //UPDATE CLICKED AIRPORT
    clickedAirport = -1;
    for(size_t i=0; i<airports.size(); ++i) {
        auto pos = camera.projToScreen(cities[i].proj);
        auto dist = SDL_distance({int(pos.x), int(pos.y)}, mousePos);
        if(dist < getRelativeRadius(airports[i].radius, camera.getZoom())) {
            clickedAirport = i;
            break;
        }
    }

    //UPDATE CLICKED ROUTE
    clickedRoute = -1;
    #pragma omp parallel for shared(clickedRoute)
    for(size_t i=0; i<routes.size(); ++i) {
        if(air::routeClicked(camera, routes[i], mousePos))
            clickedRoute = i;
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
            if(currentRoute.route.a == -1)
                currentRoute.route.a = clickedAirport;
            else if(currentRoute.route.a != clickedAirport)
                currentRoute.route.b = clickedAirport;
        } else {
            currentRoute.route.b = -1;
        }
    } else {
        if(currentRoute.route.a != -1 && currentRoute.route.b != -1)
            this->addRoute(std::move(currentRoute.route), player);
        currentRoute = CurrentRoute();
    }

    if(currentRoute.route.a != -1) {
        Coord c1 = cities[currentRoute.route.a].coord;
        Coord c2 = currentRoute.route.b==-1? camera.screenToCoords(mousePos) : cities[currentRoute.route.b].coord;
        currentRoute.route.lenght = mtsDistance(c1, c2);
        currentRoute.route.points = getPathProjs(camera, c1, c2);
        currentRoute.price = air::routePrice(currentRoute.route) * player.getDifficulty();
        currentRoute.color = (currentRoute.price > player.getCash())? SDL_RED : SDL_WHITE;
    }
}

void air::AirportManager::render(const Camera& camera, float frameProgress) const {
    for(const auto& r: routes)
        renderRoutePath(camera, r);

    for(int i=0; i<int(airports.size()); ++i)
        renderAirport(camera, airports[i], cities[i]);

    for(const auto& r: routes)
        renderRoutePlanes(camera, r, frameProgress);

    if(currentRoute.route.a != -1) {
        Coord c1 = cities[currentRoute.route.a].coord;
        Coord c2 = currentRoute.route.b==-1? camera.screenToCoords(mousePos) : cities[currentRoute.route.b].coord;
        SDL_SetRenderDrawColor(camera.getSDL(), currentRoute.color.r, currentRoute.color.g, currentRoute.color.b, currentRoute.color.a);
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

        camera.renderText(std::to_string(currentRoute.price), mousePos.x, mousePos.y - 36, 32, FC_ALIGN_CENTER, currentRoute.color);
    }

    auto t = std::format("\nZoom level: {}\n\n\n{}", camera.getZoom(), routes.size());
    camera.renderText(t, 0, 0, 32, FC_ALIGN_LEFT, SDL_WHITE);
}

bool air::AirportManager::addRoute(Route&& r, Player& player) {
    if(std::find(networkAdjList[r.a].begin(), networkAdjList[r.a].end(), r.b) != networkAdjList[r.a].end())
        return true;
    
    if(!player.spend(currentRoute.price))
        return false;
    
    auto& route = routes.emplace_back(r);
    
    airports[route.a].routeIndexes.emplace_back(routes.size() -1);
    airports[route.b].routeIndexes.emplace_back(routes.size() -1);
    
    networkAdjList[route.a].emplace_back(route.b);
    networkAdjList[route.b].emplace_back(route.a);
    
    updatePaths();
    addPlane(route);
    
    return true;
}

void air::AirportManager::addAirport(City&& c) {
    auto& city = cities.emplace_back(c);
    
    AirportData data;
    if(city.capital) data.radius = 2.0f;
    else if(city.population > 1e6) data.radius = 1.6f;
    else data.radius = 1.2f;
    
    airports.emplace_back(std::move(data));
}

void air::AirportManager::addPlane(Route& route) {
    Plane p;
    p.t = 0.0f;
    p.speed = MTS_PER_TICK_PER_LEVEL[route.level] / route.lenght;
    
    route.planes.emplace_back(p);
    landPlane(nullptr, route.planes.back(), route, false);
}

void air::AirportManager::updatePaths() {
    parentTree.resize(airports.size());
    networkAdjList.resize(airports.size());
    
    #pragma omp parallel for
    for(int i=0; i<int(airports.size()); ++i) {
        parentTree[i] = searchPath(i, networkAdjList);
    }
}

void air::AirportManager::landPlane(Player* player, Plane &plane, Route& route, bool inverted) {
    int a = route.a, b = route.b;
    if(inverted)
        std::swap(a, b);
    
    auto peopleTotal = airports[a].waiting.size() + plane.pass.size();

    // GET OFF THE PLANE
    if(player)
        player->earn(PRICE_PER_FLIGHT * plane.pass.size());
    auto &wait = airports[a].waiting;
    for(int i=0; i<int(plane.pass.size()); ++i) {
        if(plane.pass[i] != a) {
            wait.emplace_back(plane.pass[i]);
        } else {
            peopleTotal--;
        }
    }
    // GET ON THE PLANE
    int capacity = PLANE_CAPACITY_PER_LEVEL[route.level];
    plane.pass.clear();
    plane.pass.reserve(capacity);
    
    auto it = wait.begin();
    while(it != wait.end()) {
        if(int(plane.pass.size()) == capacity)
            break;

        int p = *it;
        if(getNextAirport(a, p) == b) {
            plane.pass.push_back(p);
            
            it = wait.erase(it);
        } else {
            ++it;
        }
    }

    assert(peopleTotal == wait.size() + plane.pass.size());
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
