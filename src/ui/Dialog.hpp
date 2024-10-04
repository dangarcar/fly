#pragma once

#include <string>
#include "../engine/InputEvent.h"
#include "../engine/Gradient.h"

class Camera;
class Map;
class Player;

struct Button {
    SDL_Rect localRect, globalRect;
    SDL_Color color, hoverColor, disabledColor;
    
    bool hovered = false, disabled = false;
    
    std::string text;
    int fontSize = 8;
    SDL_Color textColor = SDL_WHITE;
};

class Dialog {
public:
    //Returns true if the event has been consumed or false otherwise
    virtual bool handleInput(const InputEvent& event);
    virtual void render(const Camera& camera);
    
    void renderButton(const Button& btn, const Camera& camera) const;
    SDL_Rect getBB(const Button& btn) const { return SDL_Rect { btn.localRect.x + dialog.x, btn.localRect.y + dialog.y, btn.localRect.w, btn.localRect.h }; }

    bool mustDie() const { return die; }
    SDL_Rect getRect() const { return dialog; }

protected:
    SDL_Rect dialog;
    bool die = false;
};