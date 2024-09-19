#include "Event.hpp"

namespace Event {
    void EventManager::launchEvent(const SDL_Event& event, SDL_Window& window) {
        switch (event.type) {
            case SDL_QUIT:
                publish<QuitEvent>({});
            break;

            case SDL_KEYDOWN:
                publish<KeyPressedEvent>({ event.key.keysym.sym });
            break;

            case SDL_WINDOWEVENT:
                switch(event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_MAXIMIZED: {
                        int oldW = width, oldH = height;
                        SDL_GetWindowSize(&window, &width, &height);
                        publish<WindowResizedEvent>({ width, height, oldW, oldH });
                    } break;
                } 
            break;

            case SDL_MOUSEWHEEL:
                publish<MouseWheelEvent>({ event.wheel.y, mousePos });
            break;

            case SDL_MOUSEMOTION: {
                auto newPos = SDL_Point { event.motion.x, event.motion.y };
                if(leftDown)
                    publish<DragEvent>({ mousePos, newPos });
                else
                    publish<MouseMoveEvent>({ mousePos, newPos });

                mousePos = newPos;
            } break;

            case SDL_MOUSEBUTTONUP:
                if(oldMousePos.x == mousePos.x && oldMousePos.y == mousePos.y)
                    publish<ClickEvent>({ mousePos, event.button.button });

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
    }
}