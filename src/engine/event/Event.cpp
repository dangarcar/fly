#include "Event.hpp"
#include "EventManager.hpp"

namespace Event {
    void EventManager::launchEvent(const SDL_Event& event, SDL_Window& window) {
        switch (event.type) {
            case SDL_QUIT:
                publish<QuitEvent>(QuitEventData{});
            break;

            case SDL_KEYDOWN:
                publish<KeyPressedEvent>(KeyPressedEventData {event.key.keysym.sym});
            break;

            case SDL_WINDOWEVENT:
                switch(event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_MAXIMIZED: {
                        int oldW = width, oldH = height;
                        SDL_GetWindowSize(&window, &width, &height);
                        publish<WindowResizedEvent>(WindowResizedEventData { width, height, oldW, oldH });
                    } break;
                } 
            break;

            case SDL_MOUSEWHEEL:
                publish<MouseWheelEvent>(MouseWheelEventData { event.wheel.y, mousePos });
            break;

            case SDL_MOUSEMOTION: {
                auto newPos = SDL_Point { event.motion.x, event.motion.y };
                if(leftDown)
                    publish<DragEvent>(DragEventData{ mousePos, newPos });
                else
                    publish<MouseMoveEvent>(MouseMoveEventData{ mousePos, newPos });

                mousePos = newPos;
            } break;

            case SDL_MOUSEBUTTONUP:
                if(oldMousePos.x == mousePos.x && oldMousePos.y == mousePos.y)
                    publish<ClickEvent>(ClickEventData{mousePos, event.button.button});

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