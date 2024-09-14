#pragma once

#include "../include/json.hpp"

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <optional>

#include "Camera.hpp"
#include "Country.hpp"
#include "LabelManager.hpp"

#define COUNTRIES_DATA_FILE "./resources/countries.json"
#define MESH_DATA_FILE "./resources/mesh.bin"
#define AIRPORTS_DATA_FILE "./resources/airports.json"

class Map: public GameObject {
public:
    Map(const Camera& cam): camera(cam), labelManager(cam) {}

    void projectVertices();

    void handleEvents(const Event& event) override;
    void update() override;
    void render(const Renderer& renderer) override;
    void load(const Renderer& renderer) override;

    bool isIntersecting(const Polygon& p, const glm::vec2& v) const;
    std::optional<City> getRandomCity();

private:
    std::vector<float> vertices;
    std::vector<int32_t> triangles;
    std::map<std::string, Country> countries;
    
    std::vector<float> projectedVertices;

    LabelManager labelManager;
    const Camera& camera;
    
    bool renderBoxes = false;
    Country* targetCountry = nullptr;
    SDL_Point mousePos;
};

//Returns number of triangles loaded
void Map::load(const Renderer& renderer) {
    labelManager.load(renderer);

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
            p.boundingBox = createBoundingBox({box[0][0], box[0][1]}, {box[1][0], box[1][1]}, camera);

            c.mesh.push_back(p);
        }

        countries[k] = std::move(c);
    }

    //CITIES
    std::ifstream airportFile(AIRPORTS_DATA_FILE);
    json airportData = json::parse(airportFile);

    for(auto& [k, v]: airportData.items()) {
        for(auto& e: v) {
            City c;

            c.name = e["name"].template get<std::string>();
            c.population = e["population"].template get<int>();
            c.zoom = e["zoom"].template get<float>();
            auto coord = e["coords"].template get<std::vector<float>>();
            c.proj = camera.coordsToProj({coord[0], coord[1]});
            c.capital = e["capital"].template get<bool>();

            countries[k].cities.push_back(c);
        }
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

    projectVertices();
}

void Map::projectVertices() {
    projectedVertices.resize(vertices.size());
    
    for(int i=0; i*2<vertices.size(); ++i) {
        auto proj = camera.coordsToProj({vertices[2*i], vertices[2*i + 1]});
        projectedVertices[2*i] = proj.x;
        projectedVertices[2*i + 1] = proj.y;
    }
}

void Map::render(const Renderer& renderer) {
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

    labelManager.render(renderer);

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

void Map::handleEvents(const Event& event) {
    if(auto keyevent = std::get_if<KeyPressedEvent>(&event)) {
        if(keyevent->keycode == SDLK_q)
            renderBoxes = !renderBoxes;
    }

    else if(auto clickevent = std::get_if<ClickEvent>(&event)) {
        if(clickevent->button == SDL_BUTTON_LEFT) {
            if(targetCountry && targetCountry->state != CountryState::BANNED) {
                if(targetCountry->state != CountryState::UNLOCKED) {
                    targetCountry->state = CountryState::UNLOCKED;
                }
            }
        }
    }

    else if(auto resizedevent = std::get_if<WindowResizedEvent>(&event)) {
        projectVertices();
    }

    else if(auto moveevent = std::get_if<MouseMoveEvent>(&event)) {
       mousePos = moveevent->newPos;
    }

    else if(auto dragevent = std::get_if<DragEvent>(&event)) {
       mousePos = dragevent->newPos;
    }
}

void Map::update() {
    auto mouseProj = camera.screenToProj(mousePos);
    targetCountry = nullptr;

    Polygon* hoveredPolygon = nullptr; //There can be more than one at a time (Thanks Lesotho)
    for(auto& [name, country]: countries) {
        if(country.state == CountryState::HOVERED)
            country.state = CountryState::LOCKED;

        for(auto& pol: country.mesh) {
            if(isIntersecting(pol, mouseProj)) {
                if(hoveredPolygon && hoveredPolygon->boundingBox.area() < pol.boundingBox.area())
                    continue;
                targetCountry = &country;
                hoveredPolygon = &pol;
                break;
            }
        }
    }

    if(targetCountry && targetCountry->state == CountryState::LOCKED)
        targetCountry->state = CountryState::HOVERED;
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

std::optional<City> Map::getRandomCity() {
    std::optional<City> city;
    int tries = 0;

    do {
        auto it = countries.begin();
        std::advance(it, rand() % countries.size());

        if(it->second.state == CountryState::UNLOCKED && it->second.currentCity < it->second.cities.size()) {
            city = it->second.cities[it->second.currentCity];
            it->second.currentCity++;
        }
    } while(!city.has_value() && tries++ < 100);

    return city;
}
