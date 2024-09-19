#pragma once

#include "InputEvent.h"

class Renderer;
class Window;

class Scene {
public:
    virtual void handleInput(const InputEvent& event) = 0;
    
    virtual void start(const Window& window) = 0;
    virtual void update() = 0;
    virtual void render() = 0;

    virtual int getTicksPerSecond() const = 0;
    virtual Renderer& getRenderer() = 0; 

    virtual ~Scene() {}
};