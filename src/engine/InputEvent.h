#pragma once

#include <SDL.h>
#include <variant>

struct KeyPressedEvent{ SDL_Keycode keycode; };
struct ClickEvent{ SDL_Point clickPoint; int button; };
struct MouseUpEvent{ SDL_Point upPoint; int button; };
struct MouseMoveEvent{ SDL_Point oldPos, newPos; bool leftDown; };
struct MouseWheelEvent{ int direction; SDL_Point mousePos; };
struct WindowResizedEvent{ int width, height, oldWidth, oldHeight; };

using InputEvent = std::variant<std::monostate, KeyPressedEvent, ClickEvent, MouseMoveEvent, MouseWheelEvent, WindowResizedEvent, MouseUpEvent>;
