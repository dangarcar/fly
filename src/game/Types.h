#pragma once

#include <string>
#include <glm/glm.hpp>

constexpr long DEFAULT_CITY_PRICE = 2000;

struct City {
    std::string name;
    int population;
    glm::vec2 proj;
    bool capital;
    std::string country;
};

struct Coord { float lon, lat; };

inline int mtsDistance(Coord a, Coord b) {
    const auto RADIUS = 6371009;

    auto sinlat = std::sin( glm::radians(b.lat - a.lat) / 2 );
    auto sinlon = std::sin( glm::radians(b.lon - a.lon) / 2 );

    auto f = sinlat*sinlat + std::cos(glm::radians(a.lat)) * std::cos(glm::radians(b.lat)) * sinlon*sinlon;
    auto c = 2 * std::atan2(std::sqrt(f), std::sqrt(1 - f));

    return RADIUS * c;
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
    std::vector<Polygon> mesh;
};
