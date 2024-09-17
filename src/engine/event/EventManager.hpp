#pragma once

#include <functional>
#include <vector>
#include <memory>

#include "Event.hpp"

namespace Event {
    template<class Evt, class ... Rest>
    struct Shouter : public Shouter<Evt>, public Shouter<Rest...> {
        using Shouter<Evt>::listen;
        using Shouter<Evt>::publish;
        using Shouter<Rest ...>::listen;
        using Shouter<Rest ...>::publish;
    };

    template<class Evt>
    class Shouter<Evt> {
    private:
        std::vector<typename Evt::handler> handlers;

    public:
        template<class T>
        requires(std::is_same_v<Evt, T>) 
        void publish(Evt::data&& args) {
            for(auto& handler: handlers) {
                std::invoke(handler, args);
            }
        }

        template<class T>
        requires(std::is_same_v<Evt, T>)
        void listen(typename Evt::handler handler) {
            handlers.push_back(handler);
        }
    };
    
    class EventManager: public Shouter<QuitEvent, KeyPressedEvent, ClickEvent, DragEvent, MouseMoveEvent, MouseWheelEvent, WindowResizedEvent> {
    public:
        EventManager(int width, int height): width(width), height(height) {
            leftDown = false;
        }

        void launchEvent(const SDL_Event& event, SDL_Window& window);
    
    private:
        SDL_Point mousePos, oldMousePos;
        bool leftDown;
        int width, height;
    };
};