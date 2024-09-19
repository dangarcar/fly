#pragma once

#include "Dialog.hpp"
#include "../engine/Log.hpp"
#include <memory>
#include <vector>

class UIManager {
public:
    void registerEvents(Event::EventManager& manager) {
        manager.listen<Event::ClickEvent>([this, &manager](Event::ClickEvent::data e){
            if(!dialog)
                return true;

            auto rect = dialog->getRect();
            if(SDL_PointInRect(&e.clickPoint, &rect) == SDL_FALSE 
            || dialog->update(e.clickPoint, e.button == SDL_BUTTON_LEFT, manager) == false)
                dialog.reset(nullptr);

            return false;
        }, 1000);

        manager.listen<Event::MouseMoveEvent>([this, &manager](Event::MouseMoveEvent::data e){
            if(!dialog)
                return true;

            dialog->update(e.newPos, false, manager);

            return false;
        }, 1000);

        manager.listen<Event::UnlockCountryRequest>([this](Event::UnlockCountryRequest::data e){
            dialog = std::make_unique<UnlockCountryDialog>(e.country, e.code);
            return false;
        }, 1000);
    }

    void render(const Camera& camera) {
        if(!dialog) 
            return;

        dialog->render(camera);
    }

private:
    std::unique_ptr<Dialog> dialog;

};
