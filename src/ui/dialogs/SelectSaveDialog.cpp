#include "SelectSaveDialog.hpp"

#include "../../scenes/LoadScene.hpp"

bool SelectSaveDialog::handleInput(const InputEvent& event) {
    if(Dialog::handleInput(event))
        return true;
    
    if(auto* keyevent = std::get_if<KeyPressedEvent>(&event)) {
        if(keyevent->keycode == SDLK_UP) {
            index = (index + size - 1) % size;
        } else if(keyevent->keycode == SDLK_DOWN) {
            index = (index + 1) % size;
        } else if(keyevent->keycode == SDLK_RETURN){
            out = index;
            die = true;
        }
    }

    return true;
}

void SelectSaveDialog::render(const Renderer& renderer) {
    Dialog::render(renderer);

    renderer.renderText("SAVES", dialog.x + 10, dialog.y + 10, 64, FC_ALIGN_LEFT, SDL_WHITE);

    if(size <= 0) {
        renderer.renderText("NO SAVES", dialog.x + 10, dialog.y + 100, 40, FC_ALIGN_LEFT, SDL_WHITE);
    }

    for(int i=0; i<size; ++i) {
        SDL_Rect rect{dialog.x + 10, dialog.y + 100 + 70*i, dialog.w-20, 60};
        if(index == i)
            SDL_SetRenderDrawColor(&renderer.getSDL(), 50, 50, 50, SDL_ALPHA_OPAQUE);
        else
            SDL_SetRenderDrawColor(&renderer.getSDL(), 80, 80, 80, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(&renderer.getSDL(), &rect);

        renderer.renderText(std::to_string(i+1), rect.x + 5, rect.y + 10, 40, FC_ALIGN_LEFT, SDL_WHITE);
        
        if(i < int(saves.size())) {
            auto str = std::format("Last played: {:%Y/%m/%d %H:%M}", std::chrono::system_clock::from_time_t(saves[i].lastPlayed));
            renderer.renderText(str, rect.x + rect.w - 10, rect.y + 10, 20, FC_ALIGN_RIGHT, SDL_WHITE);

            str = std::format("Progress: {}", int(saves[i].progress * 100));
            renderer.renderText(str, rect.x + rect.w - 10, rect.y + 30, 20, FC_ALIGN_RIGHT, SDL_WHITE);
        } else {
            renderer.renderText("Unused", rect.x + rect.w - 10, rect.y + 10, 20, FC_ALIGN_RIGHT, SDL_SILVER);
        }
    }
}
