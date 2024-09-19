#pragma once

namespace Event { class EventManager; };
class Renderer;
class Window;

class Scene {
public:
    virtual void registerEvents(Event::EventManager& manager) = 0;
    
    virtual void start(const Window& window) = 0;
    virtual void update() = 0;
    virtual void render() = 0;

    virtual int getTicksPerSecond() const = 0;
    virtual Renderer& getRenderer() = 0; 

    virtual ~Scene() {}
};