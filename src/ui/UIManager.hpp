#pragma once

#include "Dialog.hpp"
#include <memory>
#include <list>
#include <cassert>

#include "../engine/Renderer.hpp"

class UIManager {
public:
    template<typename T>
    void addDialog(T&& dialog) {
        dialogs.emplace_back(std::make_unique<T>(dialog));
    }

    bool handleInput(const InputEvent& event) {
        if(dialogs.empty())
            return false;
        
        return dialogs.back()->handleInput(event);
    }

    void render(const Renderer& renderer) {
        for(auto& dialog: dialogs)
            dialog->render(renderer);
    }

    void update() {
        for(auto it=dialogs.begin(); it!=dialogs.end(); ++it) {
            if((*it)->mustDie())
                it = dialogs.erase(it);
        }
    }

    bool dialogShown() const { return !dialogs.empty(); }
    void removeDialogs() { dialogs.clear(); }

private:
    std::list<std::unique_ptr<Dialog>> dialogs;

};
