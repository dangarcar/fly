#pragma once

#include <string>
#include <glm/glm.hpp>

constexpr int DEFAULT_TICKS_PER_SECOND = 15;
constexpr long DEFAULT_CITY_PRICE = 2000;
constexpr int EARTH_RADIUS = 6371009;

struct Coord { float lon, lat; };

struct City {
    std::string name;
    int population;
    bool capital;
    std::string country;
    
    Coord coord;
    glm::vec2 proj;
};

inline int mtsDistance(Coord a, Coord b) {
    auto sinlat = glm::sin( glm::radians(b.lat - a.lat) / 2 );
    auto sinlon = glm::sin( glm::radians(b.lon - a.lon) / 2 );

    auto f = sinlat*sinlat + glm::cos(glm::radians(a.lat)) * glm::cos(glm::radians(b.lat)) * sinlon*sinlon;
    auto c = 2 * glm::atan(glm::sqrt(f), glm::sqrt(1 - f));

    return EARTH_RADIUS * c;
}

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
    std::pair<int, int> meshIndex;
};
