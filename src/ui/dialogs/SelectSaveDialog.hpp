#pragma once

#include "../Dialog.hpp"
#include "../../game/GameSaver.hpp"

class Window;

class SelectSaveDialog: public Dialog {
public:
    SelectSaveDialog(int& outIndex, Window& window, const std::vector<GameSaver::Save>& saves, bool complete): 
        out(outIndex), window(window), saves(saves), complete(complete)
    {
        dialog = SDL_Rect{0, 0, 500, 600};
        size = complete? GameSaver::MAX_SAVES : saves.size();
    }

    bool handleInput(const InputEvent& event) override;
    void render(const Renderer& renderer) override;

private:
    int index = 0;
    int size;

    int& out;
    Window& window;
    std::vector<GameSaver::Save> saves;
    bool complete;
};
