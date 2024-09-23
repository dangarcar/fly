#include "Game.hpp"

#include "../engine/Utils.h"
#include "../engine/Window.hpp"

void Game::handleInput(const InputEvent& event) {
    if(uiManager.handleInput(event)) 
        return;

    if(airManager.handleInput(event))
        return;

    camera.handleInput(event);
    player.handleInput(event);

    map.handleInput(event, camera, uiManager, player);
}

void Game::update() {
    uiManager.update();
    
    if(paused) return;

    player.update();
    map.update(camera);
    airManager.update(map.getCitySpawner(), camera);
}

void Game::render() {
    map.render(camera);
    airManager.render(camera);
    player.render(camera);

    uiManager.render(camera);

    timeFps();
}

void Game::timeFps() {
    framesDrawn++;
    auto text = std::format("{} -> {}ms", framesPerMs * 1000, 1 / framesPerMs);
    camera.renderText(text, 0, 0, 32, FC_ALIGN_LEFT, SDL_WHITE);

    if(fpsTimer.elapsedMillis() >= 500) {
        framesPerMs = framesDrawn / fpsTimer.elapsedMillis();        
        fpsTimer.reset();
        framesDrawn = 0;
    }
}

void Game::start(const Window& window) {
    if(!camera.start(window.getSDL()))
        writeError("Renderer couldn't start: %s\n", SDL_GetError());

    map.load(camera);
}