#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "../engine/ShaderProgram.hpp"
#include "../game/Types.h"
#include <SDL.h>

struct Label {
    Coord coord;
    float angle;
    float size;
    std::string name;
};

struct CountryRender {
    std::string country;
    unsigned VAO, VBO, EBO;
    int elements = 0, vertices = 0;
};

class Camera;

class MapRenderer {
    inline static const std::filesystem::path LABELS_DATA_FILE = "./resources/labels.json";
    inline static const std::filesystem::path VERTEX_SHADER_SRC = "./src/shaders/map.vs";
    inline static const std::filesystem::path FRAGMENT_SHADER_SRC = "./src/shaders/map.fs";

public:
    MapRenderer(): shader(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC) {}

    void load(
        const std::vector<float>& projectedVertices, 
        const std::vector<int32_t>& triangles, 
        const std::vector<Polygon>& polys, 
        const std::unordered_map<std::string, Country>& countries
    );

    void render(const Camera& camera, const std::unordered_map<std::string, SDL_Color>& countryColors) const;

private:
    std::unordered_map<std::string, Label> labels;
    std::vector<CountryRender> renders;
    
    ShaderProgram shader;
    int projectionLoc = -1, colorLoc = -1;

};
