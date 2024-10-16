#pragma once

#include <SDL.h>
#include <memory>
#include <variant>

#include "Utils.h"
#include "Scene.h"

#define FIXED_UPDATE 1

class Window {
public:
    static constexpr int DEFAULT_SCREEN_WIDTH = 1280;
    static constexpr int DEFAULT_SCREEN_HEIGHT = 720;

public:
    Window(): window(nullptr, &SDL_DestroyWindow), renderer(nullptr, &SDL_DestroyRenderer) {}
    ~Window() { SDL_Quit(); }
    
    int start(bool fullscreen);
    
    void run();
    void kill() { alive = false; }

    bool isAlive() const { return alive; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void setScene(std::unique_ptr<Scene> scene) { 
        this->scene = std::move(scene);
    }

    SDL_Window& getSDLWindow() const { return *window; }
    SDL_Renderer& getSDLRenderer() const { return *renderer; }

private:
    InputEvent getInputEvent();

private:
    std::unique_ptr<Scene> scene;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;
    
    bool alive = true;
    int width = DEFAULT_SCREEN_WIDTH, height = DEFAULT_SCREEN_HEIGHT;

    bool leftDown = false;
    SDL_Point mousePos, oldMousePos;
};