#include "PauseGameDialog.hpp"

#include "../../game/Game.hpp"
#include "../../scenes/StartMenu.hpp"

PauseGameDialog::PauseGameDialog(Game& game): game(game) {
    background = {0, 0, 0, 0};
    dialog = SDL_Rect {0, 0, 400, 400};

    continueButton.textColor = saveButton.textColor = exitButton.textColor = SDL_BLACK;
    continueButton.fontSize = saveButton.fontSize = exitButton.fontSize = 30;

    continueButton.text = "Continue";
    continueButton.localRect = SDL_Rect{0, 100, 400, 50};

    saveButton.text = "Save";
    saveButton.localRect = SDL_Rect{0, 180, 400, 50};

    exitButton.text = "Exit";
    exitButton.localRect = SDL_Rect{0, 260, 400, 50};
}

bool PauseGameDialog::handleInput(const InputEvent& event) {
    if(auto* keyevent = std::get_if<KeyPressedEvent>(&event)) {
        if(keyevent->keycode == SDLK_ESCAPE)
            die = true;
    }

    if(auto* clickevent = std::get_if<ClickEvent>(&event)) {
        if(clickevent->button == SDL_BUTTON_LEFT) {
            if(continueButton.isClickable()) {
                die = true;
                game.paused = false;
            } 
            else if(saveButton.isClickable()) { //TODO:

            }
            else if(exitButton.isClickable()) {
                auto startMenu = std::make_unique<StartMenu>(game.getWindow());
                startMenu->start();
                game.getWindow().setScene(std::move(startMenu));
            }
        }
    }

    if(auto* moveevent = std::get_if<MouseMoveEvent>(&event)) {
        continueButton.updateHover(moveevent->newPos);
        saveButton.updateHover(moveevent->newPos);
        exitButton.updateHover(moveevent->newPos);
    }

    return true;
}

void PauseGameDialog::render(const Renderer& renderer) {
    Dialog::render(renderer);

    continueButton.render(renderer, dialog);
    saveButton.render(renderer, dialog);
    exitButton.render(renderer, dialog);
}
