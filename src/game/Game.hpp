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

struct GameSave;

class Game : public Scene, Serializable<GameSave> {
private:
    static constexpr auto DEFAULT_GAME_FILE = "./resources/game.json";

public:
    static constexpr int MAX_FAST_FORWARD = 5;

    Game(Window& window): window(window), camera(window.getRenderer()) {
        //writeLog("Hello game\n");
    }
    ~Game() {
        //writeLog("Bye game\n");
    };

    GameSave serialize() const override;
    void deserialize(const GameSave& save) override;

    void handleInput(const InputEvent& event) override;
    
    void start() override;
    void update() override;
    void render(float frameProgress) override;

    long getTicksPerSecond() const override { return player.getFastForward() * DEFAULT_TICKS_PER_SECOND; }
    
    bool paused = false;

    Window& getWindow() { return window; }

private:
    void timeFps();
    void renderDebugInfo();

private:
    Window& window;
    UIManager uiManager;
    
    Camera camera;
    Player player;
    AirportManager airManager;
    Map map;

    long currentTick = 0L;

    Timer fpsTimer;
    int framesDrawn = 0;
    float framesPerMs = 0.0;
};

struct GameSave {
    long currentTick;

    CameraSave camera;
    PlayerSave player;
    air::AirportSave airport;
    MapSave map;

    std::vector<std::pair<std::string, int>> citiesIndexes;
};
