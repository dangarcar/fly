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

    if(airManager.handleInput(event, player, uiManager))
        return;

    if(player.handleInput(event))
        return;

    if(!uiManager.dialogShown())
        camera.handleInput(event);
    
    map.handleInput(event, camera, uiManager, player);
}

void Game::update() {
    timeFps();
    currentTick++;
    uiManager.update();
    
    if(paused || uiManager.dialogShown()) 
        return;

    player.update();
    map.update(camera);
    airManager.update(map.getCitySpawner(), camera, player, uiManager);
}

void Game::render(float frameProgress) {
    renderDebugInfo();
    return; //FIXME:

    map.render(camera);
    airManager.render(camera, (paused || uiManager.dialogShown())? 0.0f: frameProgress);
    player.render(camera, currentTick);

    uiManager.render(camera);

    renderDebugInfo();
}

void Game::timeFps() {
    framesDrawn++;
    if(fpsTimer.elapsedMillis() >= 500) {
        framesPerMs = framesDrawn / fpsTimer.elapsedMillis();        
        fpsTimer.reset();
        framesDrawn = 0;
    }
}

void Game::renderDebugInfo() {
    auto text = std::format("{} -> {}ms\n", framesPerMs * 1000, 1 / framesPerMs);
    text += std::format("Countries {}\n", player.stats.countries);
    text += std::format("Population {:.02f}M\n", double(player.stats.population) / 1e6);
    text += std::format("Airports {}\n", player.stats.airports);
    text += std::format("Routes {}\n", player.stats.routes);
    text += std::format("Planes {}\n", player.stats.planes);
    text += std::format("Passengers -> arrived: {}    total: {}\n", player.stats.passengersArrived, player.stats.passengersTotal);

    camera.renderText(text, 0, 0, 32, FC_ALIGN_LEFT, SDL_WHITE);
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