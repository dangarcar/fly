#pragma once

#include <memory>

#include "../engine/Scene.h"

#include "../Player.hpp"
#include "../map/Map.hpp"
#include "../airport/AirportManager.hpp"
#include "../ui/UIManager.hpp"
#include "Camera.hpp"

class Window;
using air::AirportManager;

class Game : public Scene {
private:
    static constexpr auto DEFAULT_GAME_FILE = "./resources/game.json";

public:
    static constexpr int MAX_FAST_FORWARD = 5;

    Game(int width, int height): camera(width, height) {}
    ~Game() = default;

    void handleInput(const InputEvent& event) override;
    
    void start(const Window& window) override;
    void update() override;
    void render(float frameProgress) override;

    long getTicksPerSecond() const override { return player.getFastForward() * DEFAULT_TICKS_PER_SECOND; }
    long getCurrentTick() const override { return currentTick; }

    Renderer& getRenderer() override { return camera; }

private:
    void timeFps();

private:
    Camera camera;
    
    UIManager uiManager;
    Player player;
    AirportManager airManager;
    Map map;

    long currentTick = 0L;
    bool paused = false;

    Timer fpsTimer;
    int framesDrawn = 0;
    float framesPerMs = 0.0;
};
