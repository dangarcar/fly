#pragma once

#include <SDL.h>

#include "event/EventManager.hpp"
#include "Renderer.hpp"

class Scene {
public:
    virtual void registerEvents(Event::EventManager& manager) = 0;
    
    virtual void update() = 0;
    
    virtual void render(const Renderer& renderer) = 0;

    virtual void load(const Renderer& renderer) = 0;

    virtual int getTicksPerSecond() const = 0;

    virtual ~Scene() {}
};