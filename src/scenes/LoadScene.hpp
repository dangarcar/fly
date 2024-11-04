#pragma once

#include "../engine/Scene.h"
#include "../engine/Gradient.h"
#include "../engine/Window.hpp"

#include "../game/Game.hpp"
#include <future>
#include <functional>

class LoadScene : public Scene {
public:
    using SceneLoader = std::function< std::unique_ptr<Scene>(Window&) >;
    LoadScene(Window& window, SceneLoader loader): 
        window(window), loader(loader)
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
        Renderer& renderer = window.getRenderer();

        auto rect = renderer.getScreenViewportRect();
        renderer.fillRect(rect, SDL_Color{ 0x03, 0x19, 0x40, SDL_ALPHA_OPAQUE});
        renderer.renderText("LOADING...", rect.w/2, rect.h/2 - 64, 128, Aligment::CENTER, SDL_WHITE);
    }

    long getTicksPerSecond() const override { return 30; }

private:
    Window& window;
    SceneLoader loader;

    std::once_flag onceFlag;
};
