#pragma once

#include "../event/Event.hpp"

class Camera;

class Dialog {
public:
    virtual void render(const Camera& camera) = 0;
    virtual bool update(SDL_Point p, bool leftClick, Event::EventManager& manager) = 0;

    SDL_Rect getRect() const { return dialog; }

protected:
    SDL_Rect dialog;
};

class UnlockCountryDialog: public Dialog {
public:
    UnlockCountryDialog(const std::string& name, const std::string& code): countryName(name), countryCode(code) {
        dialog = SDL_Rect {0, 0, 450, 300};
    }

    bool update(SDL_Point p, bool leftClick, Event::EventManager& manager) override {
        return true;
    }

    void render(const Camera& camera) override;

private:
    std::string countryName, countryCode;
};
