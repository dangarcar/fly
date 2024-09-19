#pragma once

#include <SDL.h>
#include <memory>

#include "Scene.hpp"
#include "../event/Event.hpp"
#include "Timer.hpp"

class Window {
private:
    static constexpr int DEFAULT_SCREEN_WIDTH = 1280;
    static constexpr int DEFAULT_SCREEN_HEIGHT = 720;

public:
    Window(): window(nullptr, &SDL_DestroyWindow) {}
    ~Window() { SDL_Quit(); }
    
    int start();
    
    void run();
    
    void timeFPS();

    bool isAlive() const { return alive; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void setScene(std::unique_ptr<Scene> scene) { 
        this->scene = std::move(scene);
        this->scene->start(*this);   
    }

    SDL_Window& getSDL() const { return *window; }

private:
    void registerEvents();

private:
    std::unique_ptr<Scene> scene;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
    
    Event::EventManager eventManager{DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT};
    
    Timer fpsTimer;
    int framesDrawn = 0;
    
    bool alive = true;
    int width = DEFAULT_SCREEN_WIDTH, height = DEFAULT_SCREEN_HEIGHT;
};