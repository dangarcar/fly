#pragma once

#include <memory>

#include "engine/Log.hpp"
#include "engine/Scene.hpp"
#include "Player.hpp"
#include "Map.hpp"
#include "AirportManager.hpp"

class Game : public Scene {
private:
    static constexpr int TICKS_PER_SECOND = 15;

public:
    static std::unique_ptr<Game> createGame(const Window& window);

    void handleEvents(const SystemEvent& event) override;

    void update() override;

    void render(const Renderer& renderer) override;

    void load(const Renderer& renderer) override;

    int getTicksPerSecond() const override { return ticksPerSecond; }

private:
    Game(const PlayerCamera& camera): player(camera) {}

private:
    Player player;
    AirportManager airManager;
    Map map;

    int ticksPerSecond = TICKS_PER_SECOND;
};

std::unique_ptr<Game> Game::createGame(const Window& window) {
    PlayerCamera camera(window.getWidth(), window.getHeight());
 
    return std::unique_ptr<Game>(new Game(camera));
}

void Game::handleEvents(const SystemEvent& event) {
    player.handleEvents(event);
    map.handleEvents(event, player);
    airManager.handleEvents(event);
}

void Game::update() {
    map.update(player);
    airManager.update(map.getCitySpawner());
}

void Game::render(const Renderer& renderer) {
    map.render(renderer, player.getCamera());
    airManager.render(renderer, player.getCamera());
    player.render(renderer);
}

void Game::load(const Renderer& renderer) {
    map.load(renderer, player);
    airManager.load(renderer);
}
