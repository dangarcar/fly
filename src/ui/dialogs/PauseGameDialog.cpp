#include "PauseGameDialog.hpp"

#include "../../game/Game.hpp"
#include "../../scenes/StartMenu.hpp"
#include "SelectSaveDialog.hpp"

PauseGameDialog::PauseGameDialog(Game& game, UIManager& uiManager): game(game), uiManager(uiManager) {
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
            else if(saveButton.isClickable()) {
                auto saves = GameSaver::getSaveNames();
                uiManager.addDialog<SelectSaveDialog>(saveIndex, game.getWindow(), saves, true);
            }
            else if(exitButton.isClickable()) {
                auto startMenu = std::make_unique<StartMenu>(game.getWindow());
                startMenu->start();
                game.getWindow().setScene(std::move(startMenu));
                return true;
            }
        }
    }

    if(auto* moveevent = std::get_if<MouseMoveEvent>(&event)) {
        continueButton.updateHover(moveevent->newPos);
        saveButton.updateHover(moveevent->newPos);
        exitButton.updateHover(moveevent->newPos);
    }

    if(saveIndex != -1) {
        writeLog("Saving game %d...", saveIndex);

        Timer t;
        auto save = game.serialize();
        GameSaver::save(save, saveIndex);
        writeLog("%.02fms\n", t.elapsedMillis());

        saveIndex = -1;
    }

    return true;
}

void PauseGameDialog::render(const Renderer& renderer) {
    Dialog::render(renderer);

    continueButton.render(renderer, dialog);
    saveButton.render(renderer, dialog);
    exitButton.render(renderer, dialog);
}
