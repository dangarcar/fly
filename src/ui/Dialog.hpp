#pragma once

#include <string>
#include "../engine/InputEvent.h"
#include "../engine/Gradient.h"

class Camera;
class Map;
class Player;

struct Button {
    SDL_Rect localRect, globalRect;
    SDL_Color color, hoverColor;
    
    bool hovered = false;
    
    std::string text;
    int fontSize = 8;
    SDL_Color textColor = SDL_WHITE;
};

class Dialog {
public:
    virtual bool handleInput(const InputEvent& event) = 0;
    virtual void render(const Camera& camera) = 0;
    
    void renderButton(const Button& btn, const Camera& camera) const;
    SDL_Rect getBB(const Button& btn) const { return SDL_Rect { btn.localRect.x + dialog.x, btn.localRect.y + dialog.y, btn.localRect.w, btn.localRect.h }; }

    bool mustDie() const { return die; }
    SDL_Rect getRect() const { return dialog; }

protected:
    SDL_Rect dialog;
    bool die = false;
};

class UnlockCountryDialog: public Dialog {
public:
    UnlockCountryDialog(const std::string& name, const std::string& code, Map& map, Player& player);

    bool handleInput(const InputEvent& event) override;
    void render(const Camera& camera) override;

private:
    Map& map;
    Player& player;
    std::string countryName, countryCode;

    Button yesButton, noButton;
};
