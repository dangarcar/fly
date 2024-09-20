#pragma once

#include <string>
#include <glm/glm.hpp>

constexpr long DEFAULT_CITY_PRICE = 2000;

struct City {
    std::string name;
    int population;
    glm::vec2 proj;
    bool capital;
};

struct Coord { float lon, lat; };

struct BoundingBox {
    glm::vec2 topLeft, bottomRight;

    float area() const {
        auto diff = topLeft - bottomRight;
        return diff.x * diff.y;
    }
};

struct Polygon {
    std::pair<int, int> vertexIndex;
    std::pair<int, int> triangleIndex;
    BoundingBox boundingBox;
};

enum class CountryState { LOCKED, UNLOCKED, BANNED, HOVERED };

struct Country {
    std::string name;
    CountryState state;
    std::vector<Polygon> mesh;
};
