#pragma once

#include <SDL.h>
#include <memory>

#include "Scene.h"
#include "Renderer.hpp"

#define FIXED_UPDATE 1

class Window {
public:
    static constexpr int DEFAULT_SCREEN_WIDTH = 1280;
    static constexpr int DEFAULT_SCREEN_HEIGHT = 720;

public:
    Window(): window(nullptr, &SDL_DestroyWindow) {}
    ~Window() { SDL_Quit(); }
    
    int start(bool fullscreen);
    
    void run();
    void kill() { alive = false; }

    bool isAlive() const { return alive; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void setScene(std::unique_ptr<Scene> scene) {
        this->nextScene = std::move(scene);
    }

    SDL_Window& getSDLWindow() const { return *window; }
    Renderer& getRenderer() { return renderer; }

private:
    InputEvent getInputEvent();

private:
    std::unique_ptr<Scene> scene, nextScene;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;

    Renderer renderer;

    bool alive = true;
    int width = DEFAULT_SCREEN_WIDTH, height = DEFAULT_SCREEN_HEIGHT;

    bool leftDown = false;
    SDL_Point mousePos, oldMousePos;
};