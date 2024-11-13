#pragma once

#include <vector>
#include <unordered_map>

#include "../engine/ShaderProgram.hpp"
#include "../engine/Gradient.h"
#include "../engine/TextureManager.hpp"
#include "../game/Types.h"

class Camera;

namespace air {
    constexpr long PRICE_PER_FLIGHT = 1;

    constexpr Gradient FULL_GRADIENT = {{0, 0xff, 0, SDL_ALPHA_OPAQUE}, {0xff, 0, 0, SDL_ALPHA_OPAQUE}};
    constexpr float AIRPLANE_SCALE = 2.3f;

    constexpr size_t AIRPORT_LEVELS = 7;
    constexpr int AIRPORT_CAPACITY_PER_LEVEL[AIRPORT_LEVELS] = { 100, 200, 500, 1000, 2000, 5000, 10000 };
    constexpr int AIRPORT_UPGRADE_COST[AIRPORT_LEVELS-1] = { 100, 300, 900, 2700, 8100, 24300 };

    struct AirportData {
        int level = 0;
        float radius;

        std::vector<int> waiting;

        std::vector<int> routeIndexes;
    };

    float getRelativeRadius(float radius, float zoom);

    class AirportRenderer {
        inline static const std::filesystem::path SPRITESHEET_SRC = "./assets/country_sheet.png";
        inline static const std::filesystem::path VERTEX_SHADER_SRC = "./src/shaders/airport.vs";
        inline static const std::filesystem::path FRAGMENT_SHADER_SRC = "./src/shaders/airport.fs";

    public:
        AirportRenderer(): shader(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC) {}

        void start(const std::unordered_map<std::string, Country>& countries);

        void render(const Camera& camera, const std::vector<AirportData>& airports, const std::vector<City>& cities) const;
    
    private:
        void renderInfo(const Camera& camera, const AirportData& airport, const City& city) const;

    private:
        std::vector<float> vertices;
        std::vector<int> indices;
        std::unordered_map<std::string, glm::vec2> textureCoords;
        glm::vec2 blackCoords;
        ShaderProgram shader;
        unsigned VAO, EBO, VBO;
        int projectionLoc = -1;

        Texture sheet;
    };
}
