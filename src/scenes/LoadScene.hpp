#pragma once

#include "../engine/Scene.h"
#include "../engine/Renderer.hpp"
#include "../engine/Gradient.h"

#include "../game/Game.hpp"
#include <future>
#include <functional>

class LoadScene : public Scene {
public:
    using SceneLoader = std::function< std::unique_ptr<Scene>(Window&) >;
    LoadScene(Window& window, Renderer&& r, SceneLoader loader): 
        window(window), renderer(std::forward<Renderer>(r)), loader(loader)
    {}
    ~LoadScene() = default;

    void handleInput([[maybe_unused]] const InputEvent& event) override {}
    void start() override {}
    void update() override {
        std::call_once(onceFlag, [&](){
            auto scene = loader(window);
            window.setScene(std::move(scene));
        });
    }
    
    void render([[maybe_unused]] float frameProgress) {
        auto rect = renderer.getScreenViewportRect();
        SDL_SetRenderDrawColor(&renderer.getSDL(), 0x03, 0x19, 0x40, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(&renderer.getSDL(), &rect);
        renderer.renderText("LOADING...", rect.w/2, rect.h/2 - 64, 128, FC_ALIGN_CENTER, SDL_WHITE);
    }

    long getTicksPerSecond() const override { return 30; }
    Renderer& getRenderer() override { return renderer; }; 

private:
    Window& window;
    Renderer renderer;
    SceneLoader loader;

    std::once_flag onceFlag;
};
