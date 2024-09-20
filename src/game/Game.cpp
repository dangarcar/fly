#include "Game.hpp"

#include "../engine/Log.h"
#include "../engine/Window.hpp"

void Game::handleInput(const InputEvent& event) {
    if(uiManager.handleInput(event)) 
        return;
    
    camera.handleInput(event);
    player.handleInput(event);

    airManager.handleInput(event);

    map.handleInput(event, camera, uiManager, player);
}

void Game::update() {
    uiManager.update();
    
    if(paused) return;

    player.update();
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