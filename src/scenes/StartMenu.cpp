#include "StartMenu.hpp"

#include "../engine/Window.hpp"
#include "../game/GameSaver.hpp"
#include "../ui/dialogs/SelectSaveDialog.hpp"
#include "LoadScene.hpp"

void StartMenu::handleInput(const InputEvent& event) {
    if(uiManager.handleInput(event))
        return;

    if(auto* clickevent = std::get_if<ClickEvent>(&event)) {
        if(clickevent->button == SDL_BUTTON_LEFT) {
            if(startButton.isClickable()) {
                auto scene = std::make_unique<LoadScene>(window, [](Window& win) {
                    auto game = std::make_unique<Game>(win);
                    game->start();
                    return game;
                });
                scene->start();
                window.setScene(std::move(scene));
                return;
            } 
            else if(loadButton.isClickable()) {
                auto saves = GameSaver::getSaveNames();
                uiManager.addDialog<SelectSaveDialog>(saveIndex, window, saves, false);
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
    startButton.textColor = loadButton.textColor = quitButton.textColor = settingsButton.textColor = SDL_BLACK;
    startButton.fontSize = loadButton.fontSize = quitButton.fontSize = settingsButton.fontSize = 40;

    auto screen = window.getRenderer().getScreenViewportRect();
    startButton.text = "New game";
    startButton.localRect = SDL_Rect { screen.w/2 - 250, 150, 500, 70};

    loadButton.text = "Load Game";
    loadButton.localRect = SDL_Rect { screen.w/2 - 250, 240, 500, 70};

    quitButton.text = "Quit";
    quitButton.localRect = SDL_Rect { screen.w/2 - 250, 330, 240, 70};

    settingsButton.text = "Settings";
    settingsButton.localRect = SDL_Rect { screen.w/2 + 10, 330, 240, 70};
}

void StartMenu::update() {
    uiManager.update();

    if(saveIndex != -1) {
        auto idx = saveIndex;
        auto scene = std::make_unique<LoadScene>(window, [idx](Window& win) {
            auto game = std::make_unique<Game>(win);
            game->start();

            auto save = GameSaver::getSave(idx);
            game->deserialize(save);
            return game;
        });
        saveIndex = -1;
        scene->start();
        window.setScene(std::move(scene));
        return;
    }
}

void StartMenu::render([[maybe_unused]] float frameProgress) {
    Renderer& renderer = window.getRenderer();

    auto screen = renderer.getScreenViewportRect();
    renderer.fillRect(screen, SDL_Color{0xb5, 0xc9, 0x9c, SDL_ALPHA_OPAQUE});

    renderer.renderText("AIRPORTS GAME", screen.w/2, 30, 96, Aligment::CENTER, SDL_BLACK);

    startButton.render(renderer, screen);
    loadButton.render(renderer, screen);
    quitButton.render(renderer, screen);
    settingsButton.render(renderer, screen);

    uiManager.render(renderer);
}

