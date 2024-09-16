#pragma once

#include <vector>
#include <string>

#include "PlayerCamera.hpp"
#include "engine/Gradient.hpp"

struct BoundingBox {
    glm::vec2 topLeft, bottomRight;

    float area() const {
        auto diff = topLeft - bottomRight;
        return diff.x * diff.y;
    }
};

BoundingBox createBoundingBox(Coord c1, Coord c2, const PlayerCamera& cam) {
    auto v1 = cam.coordsToProj(c1);
    auto v2 = cam.coordsToProj(c2);

    return BoundingBox { glm::min(v1, v2), glm::max(v1, v2) };
}

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

inline SDL_Color getCountryColor(const Country& country) {
    switch (country.state) {
        case CountryState::UNLOCKED:
            return {0xeb, 0xb6, 0x60, SDL_ALPHA_OPAQUE};
        
        case CountryState::LOCKED:
            return {0x4f, 0x46, 0x39, SDL_ALPHA_OPAQUE};
        
        case CountryState::HOVERED:
            return {0x99, 0x77, 0x3f, SDL_ALPHA_OPAQUE};

        default:
            return {0x25, 0x21, 0x1b, SDL_ALPHA_OPAQUE};
    }
}
