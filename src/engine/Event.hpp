#pragma once

#include <SDL.h>
#include <variant>
#include <string>

struct QuitEvent {};
struct KeyPressedEvent { SDL_Keycode keycode; };
struct ClickEvent { SDL_Point clickPoint; int button; };
struct DragEvent { SDL_Point oldPos, newPos; };
struct MouseMoveEvent { SDL_Point oldPos, newPos; };
struct MouseWheelEvent { int direction; SDL_Point mousePos; };
struct WindowResizedEvent { int width, height, oldWidth, oldHeight; };

using Event = std::variant<
    std::monostate, QuitEvent, KeyPressedEvent, ClickEvent, DragEvent, MouseMoveEvent, MouseWheelEvent, WindowResizedEvent
>;

std::string stringEventType(const Event& event) {
    const std::string types[] = {
        "std::monostate", "QuitEvent", "KeyPressedEvent", "ClickEvent", "DragEvent", "MouseMoveEvent", "MouseWheelEvent", "WindowResizedEvent"
    };
    return types[event.index()];
}

class EventHandler {
public:
    EventHandler(int width, int height): width(width), height(height) {
        leftDown = false;
    }

    Event getEvent(const SDL_Event& event, SDL_Window& window);

private:
    SDL_Point mousePos, oldMousePos;

    bool leftDown;

    int width, height;
};

Event EventHandler::getEvent(const SDL_Event& event, SDL_Window& window) {
    Event result = std::monostate();
    
    switch (event.type) {
        case SDL_QUIT:
            result = QuitEvent {};
        break;
    
        case SDL_KEYDOWN:
            result = KeyPressedEvent { event.key.keysym.sym };
        break;

        case SDL_WINDOWEVENT:
            switch(event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_MAXIMIZED: {
                    int oldW = width, oldH = height;
                    SDL_GetWindowSize(&window, &width, &height);
                    result = WindowResizedEvent { width, height, oldW, oldH };
                } break;
            } 
        break;

        case SDL_MOUSEWHEEL:
            result = MouseWheelEvent { event.wheel.y, mousePos };
        break;

        case SDL_MOUSEMOTION: {
            auto newPos = SDL_Point { event.motion.x, event.motion.y };
            if(leftDown)
                result = DragEvent { mousePos, newPos };
            else
                result = MouseMoveEvent { mousePos, newPos };

            mousePos = newPos;
        } break;

        case SDL_MOUSEBUTTONUP:
            if(oldMousePos.x == mousePos.x && oldMousePos.y == mousePos.y)
                result = ClickEvent {mousePos, event.button.button};

            oldMousePos = mousePos;
            if(event.button.button == SDL_BUTTON_LEFT)
                leftDown = false;
        break;

        case SDL_MOUSEBUTTONDOWN:
            oldMousePos = mousePos;
            if(event.button.button == SDL_BUTTON_LEFT)
                leftDown = true;
        break;

        default:
        break;
    }

    return result;
}