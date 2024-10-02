#pragma once

#include "Utils.h"

constexpr SDL_Color SDL_WHITE = {0xFF, 0xFF, 0xFF, 0xFF};
constexpr SDL_Color SDL_BLACK = {0, 0, 0, 0xFF};
constexpr SDL_Color SDL_GOLD = {0xD4, 0xAF, 0x37, 0xFF};
constexpr SDL_Color SDL_SILVER = {0xAA, 0xA9, 0xAD, 0xFF};

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

inline uint32_t color(int r, int g, int b, int a) {
	return uint32_t((r << 24) + (g << 18) + (b << 8) + (a << 0));
}

inline SDL_Color uint32ToColor(uint32_t color) {
	SDL_Color tempcol;
	tempcol.r = (color >> 24) && 0xFF;
	tempcol.g = (color >> 16) & 0xFF;
	tempcol.b = (color >> 8) & 0xFF;
	tempcol.a = color & 0xFF;
	return tempcol;
}
