#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include "LabelManager.hpp"
#include "CitySpawner.hpp"

#include "../engine/InputEvent.h"

#define COUNTRIES_DATA_FILE "./resources/countries.json"
#define MESH_DATA_FILE "./resources/mesh.bin"

class Camera;

class Map {
public:
    Map() = default;

    void projectVertices(const Camera& camera);

    void handleInput(const InputEvent& event, Camera& camera);
    void update(const Camera& camera);
    void render(const Camera& camera);
    void load(const Camera& camera);

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
