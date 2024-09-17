#pragma once

#include <SDL.h>
#include <variant>
#include <string>
#include <functional>

namespace Event {
    template<typename T>
    struct Shoutable {
        using handler = std::function<void(T)>;
        using data = T;
    };

    struct QuitEventData {};
    struct QuitEvent: Shoutable<QuitEventData> {};

    struct KeyPressedEventData { SDL_Keycode keycode; };
    struct KeyPressedEvent: Shoutable<KeyPressedEventData> {};

    struct ClickEventData { SDL_Point clickPoint; int button; };
    struct ClickEvent: Shoutable<ClickEventData> {};

    struct DragEventData { SDL_Point oldPos, newPos; };
    struct DragEvent: Shoutable<DragEventData> {};

    struct MouseMoveEventData { SDL_Point oldPos, newPos; };
    struct MouseMoveEvent: Shoutable<MouseMoveEventData> {};
    
    struct MouseWheelEventData { int direction; SDL_Point mousePos; };
    struct MouseWheelEvent: Shoutable<MouseWheelEventData> {};

    struct WindowResizedEventData { int width, height, oldWidth, oldHeight; };
    struct WindowResizedEvent: Shoutable<WindowResizedEventData> {};
};