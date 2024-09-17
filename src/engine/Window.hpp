#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <cstdio>
#include <iostream>
#include <chrono>
#include <memory>
#include <cassert>

#include "Log.hpp"
#include "Scene.hpp"
#include "event/Event.hpp"
#include "Timer.hpp"

#include "event/EventManager.hpp"

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
        this->scene->load(renderer);
    }

private:
    void registerEvents();

private:
    std::unique_ptr<Scene> scene;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
    
    Renderer renderer;
    Event::EventManager eventManager{DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT};
    
    Timer fpsTimer;
    int framesDrawn = 0;
    
    bool alive = true;
    int width = DEFAULT_SCREEN_WIDTH, height = DEFAULT_SCREEN_HEIGHT;
};

int Window::start() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        writeError("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    if(!IMG_Init(IMG_INIT_PNG)) {
        writeError("Image system could not be initialized! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    window.reset(SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
    if(!window) {
        writeError("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    if(!renderer.start(*window))
        return -1;

    return 0;
}

void Window::run() {
    assert(scene != nullptr);

    registerEvents();
    scene->registerEvents(eventManager);

    SDL_Event sdlEvent;
    Timer updateTimer;
    const float msPerTick = 1000.0 / scene->getTicksPerSecond();
    while (alive) {
        while (SDL_PollEvent(&sdlEvent)) {
            eventManager.launchEvent(sdlEvent, *window);
        }

        if(updateTimer.elapsedMillis() > msPerTick) {
            scene->update();
            updateTimer.reset();
        }

        renderer.clearScreen();

        scene->render(renderer);

        renderer.presentScreen();
        timeFPS();
    }
}

void Window::timeFPS() {
    framesDrawn++;

    if(fpsTimer.elapsedMillis() >= 500) {
        auto fpms = framesDrawn / fpsTimer.elapsedMillis();

        char str[32];
        sprintf(str, "%.02f -> %fms\n", fpms * 1000, 1/fpms); //FIXME::
        SDL_SetWindowTitle(window.get(), str);

        fpsTimer.reset();
        framesDrawn = 0;
    }
}

void Window::registerEvents() {
    eventManager.listen<Event::QuitEvent>([this](auto _){
        alive = false; 
    });

    eventManager.listen<Event::KeyPressedEvent>([this](Event::KeyPressedEvent::data e){
        if(e.keycode == SDLK_ESCAPE)
            alive = false;
    });

    eventManager.listen<Event::WindowResizedEvent>([this](Event::WindowResizedEvent::data e) {
        width = e.width;
        height = e.height;
        renderer.presentScreen();
    });
}