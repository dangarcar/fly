#include "StartMenu.hpp"

#include "../engine/Window.hpp"
#include "LoadScene.hpp"

void StartMenu::handleInput(const InputEvent& event) {
    if(auto* clickevent = std::get_if<ClickEvent>(&event)) {
        if(clickevent->button == SDL_BUTTON_LEFT) {
            if(startButton.isClickable()) {
                auto scene = std::make_unique<LoadScene>(window, std::move(renderer), [](Window& win) {
                    auto game = std::make_unique<Game>(win);
                    game->start();
                    return game;
                });
                scene->start();
                window.setScene(std::move(scene));
            } 
            else if(loadButton.isClickable()) {

            } 
            else if(quitButton.isClickable()) {
                window.kill();
            } 
            else if(settingsButton.isClickable()) {

            }
        }
    }

    if(auto* moveevent = std::get_if<MouseMoveEvent>(&event)) {
        startButton.updateHover(moveevent->newPos);
        loadButton.updateHover(moveevent->newPos);
        quitButton.updateHover(moveevent->newPos);
        settingsButton.updateHover(moveevent->newPos);
    }
}
    
void StartMenu::start() {
    if(!renderer.start())
        writeError("Renderer couldn't start: %s\n", SDL_GetError());

    startButton.textColor = loadButton.textColor = quitButton.textColor = settingsButton.textColor = SDL_BLACK;
    startButton.fontSize = loadButton.fontSize = quitButton.fontSize = settingsButton.fontSize = 40;

    auto screen = renderer.getScreenViewportRect();
    startButton.text = "Start game";
    startButton.localRect = SDL_Rect { screen.w/2 - 250, 150, 500, 70};

    loadButton.text = "Load Game";
    loadButton.localRect = SDL_Rect { screen.w/2 - 250, 240, 500, 70};

    quitButton.text = "Quit";
    quitButton.localRect = SDL_Rect { screen.w/2 - 250, 330, 240, 70};

    settingsButton.text = "Settings";
    settingsButton.localRect = SDL_Rect { screen.w/2 + 10, 330, 240, 70};
}

void StartMenu::update() {
    //NOTHING HERE
}

void StartMenu::render([[maybe_unused]] float frameProgress) {
    auto screen = renderer.getScreenViewportRect();
    SDL_SetRenderDrawColor(renderer.getSDL(), 0xb5, 0xc9, 0x9c, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer.getSDL(), &screen);

    renderer.renderText("AIRPORTS GAME", screen.w/2, 30, 96, FC_ALIGN_CENTER, SDL_BLACK);

    startButton.render(renderer, screen);
    loadButton.render(renderer, screen);
    quitButton.render(renderer, screen);
    settingsButton.render(renderer, screen);

    uiManager.render(renderer);
}

