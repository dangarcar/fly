#pragma once

#include "Dialog.hpp"
#include <memory>
#include <list>
#include <type_traits>

class UIManager {
public:
    template<typename T, typename ... Args>
    requires std::is_base_of_v<Dialog, T>
    void addDialog(Args&&... args) {
        dialogs.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    bool handleInput(const InputEvent& event) {
        bool consumed = false;
        for(auto& d: dialogs) {
            consumed |= d->handleInput(event);
        }

        return consumed;
    }

    void render(const Camera& camera) {
        for(auto& dialog: dialogs)
            dialog->render(camera);
    }

    void update() {
        for(auto it=dialogs.begin(); it!=dialogs.end(); ++it) {
            if((*it)->mustDie())
                it = dialogs.erase(it);
        }
    }

private:
    std::list<std::unique_ptr<Dialog>> dialogs;

};
