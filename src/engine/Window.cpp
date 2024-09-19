#include "Window.hpp"

#include <SDL_image.h>
#include <cassert>

#include "Log.hpp"
#include "Renderer.hpp"

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

        scene->getRenderer().clearScreen();
        scene->render();
        scene->getRenderer().presentScreen();
        
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
        return false;
    });

    eventManager.listen<Event::KeyPressedEvent>([this](Event::KeyPressedEvent::data e){
        if(e.keycode == SDLK_ESCAPE)
            alive = false;
        
        return true;
    });

    eventManager.listen<Event::WindowResizedEvent>([this](Event::WindowResizedEvent::data e) {
        width = e.width;
        height = e.height;
        scene->getRenderer().presentScreen();

        return true;
    });
}