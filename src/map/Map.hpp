#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>

#include "LabelManager.hpp"
#include "CitySpawner.hpp"

#include "../engine/InputEvent.h"

class Camera;
class UIManager;
class Player;

using MapSave = CitySpawnerSave;

class Map: Serializable<MapSave> {
public:
    SDL_Color seaColor, lockedColor, unlockedColor, bannedColor, hoveredColor;

private:
    inline static const std::filesystem::path COUNTRIES_DATA_FILE = "./resources/countries.json";
    inline static const std::filesystem::path MESH_DATA_FILE = "./resources/mesh.bin";
    //inline static const std::filesystem::path FLAGS_DATA_FILE = "./resources/flags.json";

public:
    Map() = default;

    void projectMap(const Camera& camera);

    void handleInput(const InputEvent& event, Camera& camera, UIManager& uiManager, Player& player);
    void update(const Camera& camera);
    void render(const Camera& camera);
    void load(Camera& camera);

    bool isIntersecting(const Polygon& p, const glm::vec2& v) const;
    void unlockCountry(const std::string& country, Player& player);
    void moveToCountry(const Country& country, Camera& camera);

    CitySpawner& getCitySpawner() { return citySpawner; }
    const CitySpawner& getCitySpawner() const { return citySpawner; }

    MapSave serialize() const override;
    void deserialize(const MapSave& save) override;

private:
    //DATA
    std::vector<float> vertices;
    std::vector<float> projectedVertices;
    std::vector<int32_t> triangles;
    std::unordered_map<std::string, Country> countries;

    //CACHED RENDERING ARRAYS
    std::vector<SDL_Vertex> vertexArray;
    std::vector<std::vector<SDL_Point>> lineArray;
    std::vector<Polygon> polygons;

    //SYSTEMS
    LabelManager labelManager;
    CitySpawner citySpawner;
    
    //MORE DATA
    bool renderBoxes = false;
    std::string targetCountry;
    SDL_Point mousePos;
};
