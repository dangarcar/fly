#include "Game.hpp"

#include "../../include/json.hpp"
#include <fstream>

#include "../engine/Utils.h"
#include "../engine/Window.hpp"

void Game::handleInput(const InputEvent& event) {
    if(auto* keyevent = std::get_if<KeyPressedEvent>(&event)) {
        if(keyevent->keycode == SDLK_p)
            paused = !paused;
    }

    if(uiManager.handleInput(event)) 
        return;

    if(airManager.handleInput(event))
        return;

    camera.handleInput(event);
    player.handleInput(event);

    map.handleInput(event, camera, uiManager, player);
}

void Game::update() {
    currentTick++;
    uiManager.update();
    
    if(paused || uiManager.dialogShown()) return;

    player.update();
    map.update(camera);
    airManager.update(map.getCitySpawner(), camera, player);
}

void Game::render(float frameProgress) {
    map.render(camera);
    airManager.render(camera, paused? 0.0f: frameProgress);
    player.render(camera, currentTick);

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

    using json = nlohmann::json;
    std::ifstream file(DEFAULT_GAME_FILE);
    auto data = json::parse(file);

    auto textures = data["textures"];
    for(auto& [k, v]: textures.items()) {
        auto path = v.template get<std::string>();
        camera.getTextureManager().loadTexture(*camera.getSDL(), k, path);
    }

    map.seaColor = hexCodeToColor(data["SEA_COLOR"].template get<std::string>());
    map.bannedColor = hexCodeToColor(data["BANNED_COLOR"].template get<std::string>());
    map.unlockedColor = hexCodeToColor(data["UNLOCKED_COLOR"].template get<std::string>());
    map.lockedColor = hexCodeToColor(data["LOCKED_COLOR"].template get<std::string>());
    map.hoveredColor = hexCodeToColor(data["HOVERED_COLOR"].template get<std::string>());

    map.load(camera);
}