#include "Window.hpp"

#include <SDL_image.h>
#include <cassert>

#include "Log.h"
#include "Scene.h"
#include "Renderer.hpp"

void Window::setScene(std::unique_ptr<Scene> ptr) {
    scene = std::move(ptr);
    scene->start(*this);   
}

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

    SDL_Event sdlEvent;
    Timer updateTimer;
    const float msPerTick = 1000.0 / scene->getTicksPerSecond();
    while (alive) {
        InputEvent event;
        while(event = getInputEvent(), event.index() != 0) {
            scene->handleInput(event);
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

InputEvent Window::getInputEvent() {
    InputEvent event;
    SDL_Event sdl;
    SDL_PollEvent(&sdl);

    switch(sdl.type) {
        case SDL_QUIT:
            alive = false;
        break;

        case SDL_KEYDOWN:
            event = KeyPressedEvent { sdl.key.keysym.sym };
        break;

        case SDL_WINDOWEVENT:
            switch(sdl.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_MAXIMIZED: {
                    int oldW = width, oldH = height;
                    SDL_GetWindowSize(window.get(), &width, &height);
                    event = WindowResizedEvent{ width, height, oldW, oldH };
                } break;
            } 
        break;

        case SDL_MOUSEWHEEL:
            event = MouseWheelEvent{ sdl.wheel.y, mousePos };
        break;

        case SDL_MOUSEMOTION: {
            auto newPos = SDL_Point { sdl.motion.x, sdl.motion.y };
            event = MouseMoveEvent{ mousePos, newPos, leftDown };

            mousePos = newPos;
        } break;

        case SDL_MOUSEBUTTONUP:
            if(oldMousePos.x == mousePos.x && oldMousePos.y == mousePos.y)
                event = ClickEvent{ mousePos, sdl.button.button };

            oldMousePos = mousePos;
            if(sdl.button.button == SDL_BUTTON_LEFT)
                leftDown = false;
        break;

        case SDL_MOUSEBUTTONDOWN:
            oldMousePos = mousePos;
            if(sdl.button.button == SDL_BUTTON_LEFT)
                leftDown = true;
        break;

        default: break;
    }

    return event;
}