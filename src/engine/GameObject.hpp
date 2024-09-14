#pragma once

#include <SDL.h>
#include <chrono>
#include "Event.hpp"

#include "Renderer.hpp"

class GameObject {
public:
    virtual void handleEvents(const Event& event) = 0;
    
    virtual void update() = 0;
    
    virtual void render(const Renderer& renderer) = 0;

    virtual void load(const Renderer& renderer) = 0;

    virtual ~GameObject() {}
};

class Timer {
public:
    Timer() { reset(); }
    void reset() { time = std::chrono::system_clock::now(); }
    float elapsedMillis() { return (std::chrono::system_clock::now() - time).count() / 1e6f; }

private:
    std::chrono::system_clock::time_point time;
};