#pragma once

#include <memory>

#include "../engine/Scene.h"

#include "../Player.hpp"
#include "../map/Map.hpp"
#include "../Airport.hpp"
#include "../ui/UIManager.hpp"
#include "Camera.hpp"

class Window;

class Game : public Scene {
private:
    static constexpr int TICKS_PER_SECOND = 150; //TODO: must change

public:
    Game(int width, int height): camera(width, height) {}
    ~Game() = default;

    void handleInput(const InputEvent& event) override;
    
    void start(const Window& window) override;
    void update() override;
    void render() override;

    int getTicksPerSecond() const override { return ticksPerSecond; }

    Renderer& getRenderer() override { return camera; }

private:
    void timeFps();

private:
    Camera camera;
    
    UIManager uiManager;
    Player player;
    AirportManager airManager;
    Map map;

    int ticksPerSecond = TICKS_PER_SECOND;
    bool paused = false;

    Timer fpsTimer;
    int framesDrawn = 0;
    float framesPerMs = 0.0;
};
