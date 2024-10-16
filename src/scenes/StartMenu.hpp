#pragma once

#include "../engine/Window.hpp"
#include "../engine/Scene.h"
#include "../engine/Renderer.hpp"
#include "../ui/UIManager.hpp"

class StartMenu : public Scene {
public:
    StartMenu(Window& window): window(window), renderer(window.getSDLRenderer(), window.getWidth(), window.getHeight()) {}
    ~StartMenu() = default;

    void handleInput(const InputEvent& event) override;
    
    void start() override;
    void update() override;
    void render(float frameProgress) override;

    long getTicksPerSecond() const override { return 30; }
    Renderer& getRenderer() override { return renderer; }

private:
    Window& window;
    Renderer renderer;
    UIManager uiManager;

    Button startButton, loadButton, quitButton, settingsButton;
};
