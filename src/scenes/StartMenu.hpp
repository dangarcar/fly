#pragma once

#include "../engine/Window.hpp"
#include "../engine/Scene.h"
#include "../ui/UIManager.hpp"
#include "../game/GameSaver.hpp"

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

    int saveIndex = -1;
    Button startButton, loadButton, quitButton, settingsButton;
};
