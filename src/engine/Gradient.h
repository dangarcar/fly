#pragma once

#include <glm/glm.hpp>
#include <SDL.h>

constexpr SDL_Color SDL_WHITE = {0xFF, 0xFF, 0xFF, 0xFF};
constexpr SDL_Color SDL_BLACK = {0, 0, 0, 0xFF};
constexpr SDL_Color SDL_GOLD = {0xD4, 0xAF, 0x37, 0xFF};
constexpr SDL_Color SDL_SILVER = {0xAA, 0xA9, 0xAD, 0xFF};

constexpr inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

class Gradient {
private:
    SDL_Color a, b;

public:
    constexpr Gradient(SDL_Color a, SDL_Color b): a(a), b(b) {}

    SDL_Color getColor(float t) const {
        return {
            (Uint8)lerp(a.r, b.r, t),
            (Uint8)lerp(a.g, b.g, t),
            (Uint8)lerp(a.b, b.b, t),
            (Uint8)lerp(a.a, b.a, t),
        };
    }
};