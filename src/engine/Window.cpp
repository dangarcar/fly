#include "Window.hpp"

#include <cassert>

#include "Renderer.hpp"
#include "Scene.h"

#include <glad/glad.h>

int Window::start(bool fullscreen) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        writeError("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    if(fullscreen) {
        SDL_DisplayMode dm;
        SDL_GetDisplayMode(0, 0, &dm);
        this->width = dm.w;
        this->height = dm.h;
        
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    window.reset(SDL_CreateWindow("Airport Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags));
    if(!window) {
        writeError("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    renderer = Renderer(width, height, *window);

    return 0;
}

void Window::run() {
    Timer updateTimer;
    while (alive) {
        if(nextScene) {
            scene = std::move(nextScene);
            nextScene = nullptr;
        }

        assert(scene != nullptr);

#ifdef FIXED_UPDATE
        float msPerTick = 1000.0 / scene->getTicksPerSecond();
#else
        float msPerTick = SDL_FLT_EPSILON;
#endif
        InputEvent event;
        while(event = getInputEvent(), event.index() != 0) {
            scene->handleInput(event);
        }

        if(updateTimer.elapsedMillis() >= msPerTick) {
            scene->update();
            updateTimer.reset();
        }

        renderer.clearScreen();
        scene->render(updateTimer.elapsedMillis() / msPerTick);
        SDL_GL_SwapWindow(window.get());
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
            else
                event = MouseUpEvent{ mousePos, sdl.button.button };
            
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