#include "Game.hpp"

#include "../engine/Window.hpp"

void Game::handleInput(const InputEvent& event) {
    camera.handleInput(event);

    if(uiManager.handleInput(event)) 
        return;
    
    player.handleInput(event);
    map.handleInput(event, camera);
    airManager.handleInput(event);
}

void Game::update() {
    if(paused) return;

    map.update(camera);
    airManager.update(map.getCitySpawner());
}

void Game::render() {
    map.render(camera);
    airManager.render(camera);
    player.render(camera);

    uiManager.render(camera);
}

void Game::start(const Window& window) {
    if(!camera.start(window.getSDL()))
        writeError("Renderer couldn't start: %s\n", SDL_GetError());

    map.load(camera);
}