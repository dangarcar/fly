#pragma once

#include "../Dialog.hpp"

class Game;

class PauseGameDialog: public Dialog {
public:
    PauseGameDialog(Game& game);

    bool handleInput(const InputEvent& event) override;
    void render(const Renderer& renderer) override;

private:
    Game& game;

    Button continueButton, saveButton, exitButton;
};
