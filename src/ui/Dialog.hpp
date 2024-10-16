#pragma once

#include <string>
#include "../engine/InputEvent.h"
#include "../engine/Gradient.h"

class Renderer;

class Button {
public:    
    void render(const Renderer& renderer, SDL_Rect parent);
    
    void setDisabled(bool condition) { disabled = condition; }
    void updateHover(SDL_Point mousePos) { hovered = SDL_PointInRect(&mousePos, &globalRect); }
    bool isClickable() const { return !disabled && hovered; };

public://PROPERTIES
    SDL_Rect localRect;
    SDL_Color color = SDL_GRAY, hoverColor = SDL_DARK_GRAY, disabledColor = SDL_SILVER;
    int fontSize = 8;
    SDL_Color textColor = SDL_WHITE;
    std::string text;

private: //STATE
    SDL_Rect globalRect;
    bool hovered = false, disabled = false;
    
};

class Dialog {
public:
    //Returns true if the event has been consumed or false otherwise
    virtual bool handleInput(const InputEvent& event);
    virtual void render(const Renderer& renderer);

    bool mustDie() const { return die; }
    SDL_Rect getRect() const { return dialog; }

protected:
    SDL_Rect dialog;
    SDL_Color background = { 100, 100, 100, SDL_ALPHA_OPAQUE };
    bool die = false;
};