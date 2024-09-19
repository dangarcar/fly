#pragma once

#include <string>
#include "../engine/InputEvent.h"

class Camera;

class Dialog {
public:
    virtual void render(const Camera& camera) = 0;
    virtual bool update(SDL_Point p, bool leftClick, const InputEvent& event) = 0;

    SDL_Rect getRect() const { return dialog; }

protected:
    SDL_Rect dialog;
};

class UnlockCountryDialog: public Dialog {
public:
    UnlockCountryDialog(const std::string& name, const std::string& code): countryName(name), countryCode(code) {
        dialog = SDL_Rect {0, 0, 450, 300};
    }

    bool update(SDL_Point p, bool leftClick, const InputEvent& event) override {
        return true;
    }

    void render(const Camera& camera) override;

private:
    std::string countryName, countryCode;
};
