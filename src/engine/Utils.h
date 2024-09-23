#pragma once

#include <cstdarg>
#include <cstdio>
#include <chrono>
#include <cmath>
#include <SDL.h>

constexpr inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

inline float SDL_distance(SDL_Point a, SDL_Point b) {
    return std::sqrt(float((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)));
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
