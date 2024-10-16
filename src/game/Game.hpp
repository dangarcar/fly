#pragma once

#include <memory>

#include "../engine/Scene.h"
#include "../engine/Window.hpp"

#include "../Player.hpp"
#include "../map/Map.hpp"
#include "../airport/AirportManager.hpp"
#include "../ui/UIManager.hpp"
#include "Camera.hpp"

using air::AirportManager;

class Game : public Scene {
private:
    static constexpr auto DEFAULT_GAME_FILE = "./resources/game.json";

public:
    static constexpr int MAX_FAST_FORWARD = 5;

    Game(Window& window): window(window), camera(window.getSDLRenderer(), window.getWidth(), window.getHeight()) {}
    ~Game() = default;

    void handleInput(const InputEvent& event) override;
    
    void start() override;
    void update() override;
    void render(float frameProgress) override;

    long getTicksPerSecond() const override { return player.getFastForward() * DEFAULT_TICKS_PER_SECOND; }

    Renderer& getRenderer() override { return camera; }
    
    bool paused = false;

    Window& getWindow() { return window; }

private:
    void timeFps();
    void renderDebugInfo();

private:
    Window& window;
    Camera camera;
    
    UIManager uiManager;
    Player player;
    AirportManager airManager;
    Map map;

    long currentTick = 0L;

    Timer fpsTimer;
    int framesDrawn = 0;
    float framesPerMs = 0.0;
};
