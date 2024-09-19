#pragma once

#include <SDL.h>
#include <string>
#include <functional>

//This looks like shit but I think it's cleaner

#define MAKE_EVENT(name, elems) struct name ## Data elems; struct name: Shoutable<name ## Data> {}

namespace Event {
    template<typename T>
    struct Shoutable {
        using handler = std::function<bool(T)>; //Return true if element should propagate
        using data = T;
    };

    //SYSTEM EVENTS
    MAKE_EVENT(QuitEvent, {});
    MAKE_EVENT(KeyPressedEvent, { SDL_Keycode keycode; });
    MAKE_EVENT(ClickEvent, { SDL_Point clickPoint; int button; });
    MAKE_EVENT(DragEvent, { SDL_Point oldPos; SDL_Point newPos; });
    MAKE_EVENT(MouseMoveEvent, { SDL_Point oldPos; SDL_Point newPos; });
    MAKE_EVENT(MouseWheelEvent, { int direction; SDL_Point mousePos; });
    MAKE_EVENT(WindowResizedEvent, { int width; int height; int oldWidth; int oldHeight; });

    //GAME EVENTS
    MAKE_EVENT(UnlockCountryRequest, { std::string country; std::string code; });
    MAKE_EVENT(UnlockedCountry, { std::string country; std::string code; });

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
        std::vector<std::pair<int, typename Evt::handler>> handlers;

    public:
        template<class T>
        requires(std::is_same_v<Evt, T>) 
        void publish(Evt::data&& args) {
            for(auto& [priority, handler]: handlers) {
                if(handler(args) == false)
                    break;
            }
        }

        template<class T>
        requires(std::is_same_v<Evt, T>)
        void listen(typename Evt::handler handler, int priority=0) {
            handlers.push_back(std::make_pair(priority, handler));
            sort(handlers.begin(), handlers.end(), [](auto& a, auto& b){ return a.first > b.first; });
        }
    };

    class EventManager: public Shouter<
        QuitEvent, KeyPressedEvent, ClickEvent, DragEvent, MouseMoveEvent, MouseWheelEvent, WindowResizedEvent, 
        UnlockCountryRequest, UnlockedCountry
    > {
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