#pragma once

#include <cstdarg>
#include <cstdio>
#include <chrono>
#include <cstdlib>
#include <SDL.h>

#include <glm/glm.hpp>

constexpr inline float flerp(float a, float b, float t) {
    return a + (b - a) * t;
}

inline float SDL_sqrdistance(SDL_Point a, SDL_Point b) {
    return float((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

inline float SDL_distance(SDL_Point a, SDL_Point b) {
    return glm::sqrt(float((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)));
}

class Timer {
public:
    Timer() { reset(); }
    void reset() { time = std::chrono::system_clock::now(); }
    float elapsedMillis() { return (std::chrono::system_clock::now() - time).count() / 1e6f; }

private:
    std::chrono::system_clock::time_point time;
};

inline void writeLog(const char* format, ...) {
    va_list args;
    va_start(args, format);

    vprintf(format, args);

    va_end(args);
}

inline void writeError(const char* format, ...) {
    va_list args;
    va_start(args, format);

    vfprintf(stderr, format, args);

    va_end(args);
}

inline SDL_Color hexCodeToColor(const std::string& str) {
    long n = std::strtoul(str.c_str() + 1, nullptr, 16);
    uint8_t r = (n & 0xFF0000) >> 16;
    uint8_t g = (n & 0x00FF00) >> 8;
    uint8_t b = n & 0x0000FF;

    return SDL_Color { r, g, b, SDL_ALPHA_OPAQUE };
}

inline std::string cutNCharacters(std::string s, int n) {
    if(int(s.length()) > n) {
        s = s.substr(0, n-1);
        if(s.back() == ' ')
            s = s.substr(0, n-2);
        s = s.append("...");
    }

    return s;
}

inline SDL_FRect toFRect(SDL_Rect rect) {
    return SDL_FRect{float(rect.x), float(rect.y), float(rect.w), float(rect.h)};
}

inline SDL_Rect toRect(SDL_FRect frect) {
    return SDL_Rect{int(frect.x), int(frect.y), int(frect.w), int(frect.h)};
}
