#pragma once

#include "../include/json.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>

#include "PlayerCamera.hpp"
#include "Country.hpp"
#include "LabelManager.hpp"
#include "CitySpawner.hpp"

#define COUNTRIES_DATA_FILE "./resources/countries.json"
#define MESH_DATA_FILE "./resources/mesh.bin"

class Map {
public:
    Map() = default;

    void projectVertices(const PlayerCamera& camera);

    void registerEvents(Event::EventManager& manager, const Player& player);
    void update(const Player& player);
    void render(const Renderer& renderer, const PlayerCamera& camera);
    void load(const Renderer& renderer, const Player& player);

    bool isIntersecting(const Polygon& p, const glm::vec2& v) const;

    void unlockCountry(std::string country);

    CitySpawner& getCitySpawner() { return citySpawner; };

private:
    //DATA
    std::vector<float> vertices;
    std::vector<float> projectedVertices;
    std::vector<int32_t> triangles;
    std::unordered_map<std::string, Country> countries;

    //SYSTEMS
    LabelManager labelManager;
    CitySpawner citySpawner;
    
    //MORE DATA
    bool renderBoxes = false;
    std::string targetCountry;
    SDL_Point mousePos;
};

//Returns number of triangles loaded
void Map::load(const Renderer& renderer, const Player& player) {
    labelManager.load(renderer);
    citySpawner.load(player.getCamera());

    //COUNTRY MESH
    using json = nlohmann::json;
    std::ifstream countryFile(COUNTRIES_DATA_FILE);
    json countryData = json::parse(countryFile);

    for(auto& [k, v]: countryData.items()) {
        Country c;
        c.name = v["name"].template get<std::string>();
        auto banned = v["banned"].template get<bool>();
        c.state = banned? CountryState::BANNED : CountryState::LOCKED;

        for(auto& e: v["mesh"]) {
            Polygon p;
            auto box = e["box"].template get<std::vector<std::vector<float>>>(); //It was not compatible with Bounding box type
            p.vertexIndex = e["vertexIndex"].template get<std::pair<int, int>>();
            p.triangleIndex = e["triangleIndex"].template get<std::pair<int, int>>();
            p.boundingBox = createBoundingBox({box[0][0], box[0][1]}, {box[1][0], box[1][1]}, player.getCamera());

            c.mesh.push_back(p);
        }

        countries[k] = std::move(c);
    }

    //MESH LOADING
    std::ifstream meshFile(MESH_DATA_FILE, std::ios::binary);
    int32_t vertexSize;
    meshFile.read(reinterpret_cast<char*>(&vertexSize), sizeof(int32_t));

    int32_t triSize;
    meshFile.read(reinterpret_cast<char*>(&triSize), sizeof(int32_t));

    vertices.clear();
    vertices.resize(vertexSize * 2);
    meshFile.read(reinterpret_cast<char*>(&vertices[0]), vertexSize * 2 * sizeof(float));

    triangles.clear();
    triangles.resize(triSize * 3);
    meshFile.read(reinterpret_cast<char*>(&triangles[0]), triSize * 3 * sizeof(int32_t));

    projectVertices(player.getCamera());
}

void Map::projectVertices(const PlayerCamera& camera) {
    projectedVertices.resize(vertices.size());
    
    for(int i=0; i*2<vertices.size(); ++i) {
        auto proj = camera.coordsToProj({vertices[2*i], vertices[2*i + 1]});
        projectedVertices[2*i] = proj.x;
        projectedVertices[2*i + 1] = proj.y;
    }
}

void Map::render(const Renderer& renderer, const PlayerCamera& camera) {
    SDL_SetRenderDrawColor(renderer.getSDL(), SEA_COLOR.r, SEA_COLOR.g, SEA_COLOR.b, SEA_COLOR.a);
    SDL_Rect rect = camera.getScreenViewportRect();
    SDL_RenderFillRect(renderer.getSDL(), &rect);

    std::vector<SDL_Vertex> vertexArray(vertices.size() / 2);
    std::vector<std::vector<SDL_Point>> lines;
    for(auto& [name, country]: countries) {
        for(auto pol: country.mesh) {
            std::vector<SDL_Point> line;

            auto [begV, endV] = pol.vertexIndex;
            for(auto i=begV; i!=endV; ++i) {
                SDL_Vertex v;
                v.color = getCountryColor(country);
                v.position = camera.projToScreen({projectedVertices[2*i], projectedVertices[2*i + 1]});
                vertexArray[i] = v;
                line.push_back(SDL_Point{int(v.position.x), int(v.position.y)});
            }

            lines.push_back(line);

            //BOX RENDER
            if(renderBoxes) {
                auto x1 = pol.boundingBox.topLeft.x, y1 = pol.boundingBox.topLeft.y;
                auto x2 = pol.boundingBox.bottomRight.x, y2 = pol.boundingBox.bottomRight.y;
                std::vector box = { camera.projToScreen({x1, y1}), camera.projToScreen({x1, y2}), camera.projToScreen({x2, y2}), camera.projToScreen({x2, y1}), camera.projToScreen({x1, y1}) };
                SDL_SetRenderDrawColor(renderer.getSDL(), 0, 0xFF, 0xFF, 0xFF);
                SDL_RenderDrawLinesF(renderer.getSDL(), box.data(), box.size());
            }
        }
    }

    SDL_RenderGeometry(renderer.getSDL(), nullptr, vertexArray.data(), vertexArray.size(), triangles.data(), triangles.size());

    labelManager.render(renderer, camera);

    SDL_SetRenderDrawColor(renderer.getSDL(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    for(auto& l: lines) {
        SDL_RenderDrawLines(renderer.getSDL(), l.data(), l.size());
    }

    auto str = std::format("Countries left: {}\nUnlocked: {}\n",
        std::accumulate(countries.begin(), countries.end(), 0, [&](int a, auto& b){ return a + (b.second.state != CountryState::BANNED); }),
        std::accumulate(countries.begin(), countries.end(), 0, [&](int a, auto& b){ return a + (b.second.state == CountryState::UNLOCKED); })
    );
    renderer.renderText(str, 0, 64, 32, FC_ALIGN_LEFT, SDL_WHITE);
}

void Map::registerEvents(Event::EventManager& manager, const Player& player) {
    manager.listen<Event::KeyPressedEvent>([this](Event::KeyPressedEvent::data e){
        if(e.keycode == SDLK_q)
            renderBoxes = !renderBoxes;
    });

    manager.listen<Event::ClickEvent>([this](Event::ClickEvent::data e){
        if(e.button == SDL_BUTTON_LEFT) {
            if(!targetCountry.empty() && countries[targetCountry].state != CountryState::BANNED) {
                if(countries[targetCountry].state != CountryState::UNLOCKED) {
                    this->unlockCountry(targetCountry);
                }
            }
        }
    });

    manager.listen<Event::WindowResizedEvent>([this, player](Event::WindowResizedEvent::data _){
        projectVertices(player.getCamera());
    });

    manager.listen<Event::MouseMoveEvent>([this](Event::MouseMoveEvent::data e){
       mousePos = e.newPos;
    });

    manager.listen<Event::DragEvent>([this](Event::DragEvent::data e){
       mousePos = e.newPos;
    });
}

void Map::update(const Player& player) {
    auto mouseProj = player.getCamera().screenToProj(mousePos);
    targetCountry.clear();

    Polygon* hoveredPolygon = nullptr; //There can be more than one at a time (Thanks Lesotho)
    for(auto& [name, country]: countries) {
        if(country.state == CountryState::HOVERED)
            country.state = CountryState::LOCKED;

        for(auto& pol: country.mesh) {
            if(isIntersecting(pol, mouseProj)) {
                if(hoveredPolygon && hoveredPolygon->boundingBox.area() < pol.boundingBox.area())
                    continue;
                targetCountry = name;
                hoveredPolygon = &pol;
                break;
            }
        }
    }

    if(!targetCountry.empty() && countries[targetCountry].state == CountryState::LOCKED)
        countries[targetCountry].state = CountryState::HOVERED;
}

bool Map::isIntersecting(const Polygon& p, const glm::vec2& v) const {
    if(v.x < p.boundingBox.topLeft.x || v.y < p.boundingBox.topLeft.y
    || v.x > p.boundingBox.bottomRight.x || v.y > p.boundingBox.bottomRight.y)
        return false;

    bool inside = false;
    auto [beg, end] = p.vertexIndex;
    int numVertices = end - beg;

    glm::vec2 p1 = { projectedVertices[2*beg], projectedVertices[2*beg + 1] }, p2;
    for(int i=1; i<=numVertices; ++i) {
        int j = beg + (i % numVertices);
        p2 = {projectedVertices[j*2], projectedVertices[j*2 + 1]};

        if(v.y > std::min(p1.y, p2.y) && v.y <= std::max(p1.y, p2.y)) {
            if(v.x <= std::max(p1.x, p2.x)) {
                auto xInter = (v.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;

                if(p1.x == p2.x || v.x <= xInter)
                    inside = !inside;
            }
        }

        p1 = p2;
    }

    return inside;
}

void Map::unlockCountry(std::string country) {
    //EventManager::publish<Event::UnlockCountry>(std::format("{} uploaded", country), 24);
    countries[country].state = CountryState::UNLOCKED;
    citySpawner.addCountry(country);
}
