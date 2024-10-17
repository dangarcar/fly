#pragma once

#include "../engine/Window.hpp"
#include "../engine/Scene.h"
#include "../ui/UIManager.hpp"

class StartMenu : public Scene {
public:
    StartMenu(Window& window): window(window) {}
    ~StartMenu() = default;

    void handleInput(const InputEvent& event) override;
    
    void start() override;
    void update() override;
    void render(float frameProgress) override;

    long getTicksPerSecond() const override { return 30; }

private:
    Window& window;
    UIManager uiManager;

    Button startButton, loadButton, quitButton, settingsButton;
};
