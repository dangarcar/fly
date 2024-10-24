#pragma once

#include "../Dialog.hpp"

class Game;
class UIManager;

class PauseGameDialog: public Dialog {
public:
    PauseGameDialog(Game& game, UIManager& uiManager);

    bool handleInput(const InputEvent& event) override;
    void render(const Renderer& renderer) override;

private:
    Game& game;
    UIManager& uiManager;

    Button continueButton, saveButton, exitButton;
    int saveIndex = -1;
};
