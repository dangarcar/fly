#pragma once

#include <string>
#include "../engine/InputEvent.h"
#include "../engine/Gradient.h"

class Camera;
class Map;
class Player;

class Button {
public:    
    void render(const Camera& camera, SDL_Rect parent);
    
    void setDisabled(bool condition) { disabled = condition; }
    void updateHover(SDL_Point mousePos) { hovered = SDL_PointInRect(&mousePos, &globalRect); }
    bool isClickable() const { return !disabled && hovered; };

public://PROPERTIES
    SDL_Rect localRect;
    SDL_Color color, hoverColor, disabledColor = SDL_SILVER;
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
    virtual void render(const Camera& camera);

    bool mustDie() const { return die; }
    SDL_Rect getRect() const { return dialog; }

protected:
    SDL_Rect dialog;
    bool die = false;
};