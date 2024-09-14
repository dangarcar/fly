#pragma once

#include "engine/Log.hpp"
#include "engine/GameObject.hpp"
#include "Camera.hpp"
#include "Map.hpp"
#include "AirportManager.hpp"

class Game : public GameObject {
public:
    Game(Camera& cam): camera(cam), airManager(cam) {}

    void handleEvents(const Event& event) override;

    void update() override;

    void render(const Renderer& renderer) override;

    void load(const Renderer& renderer) override;

private:
    Camera& camera;
    AirportManager airManager;
};

void Game::handleEvents(const Event& event) {
    camera.handleEvents(event);
    airManager.handleEvents(event);
}

void Game::update() {
    airManager.update();
}

void Game::render(const Renderer& renderer) {
    airManager.render(renderer);
}

void Game::load(const Renderer& renderer) {
    airManager.load(renderer);
}
