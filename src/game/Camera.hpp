#pragma once

#include <glm/glm.hpp>

#include "../engine/Renderer.hpp"
#include "Types.h"

namespace Event { class EventManager; }

class Camera : public Renderer {
private:
    static constexpr float MAX_ZOOM = 24.0f;
    static constexpr float ZOOM_SPEED = 0.15f;

    static constexpr Coord INITIAL_POS = {0, 30};

public:
    Camera(int w, int h): width(w), height(h) {
        pos = coordsToProj(INITIAL_POS);
    }

    glm::vec2 coordsToProj(Coord coords) const;
    Coord projToCoords(glm::vec2 proj) const;

    SDL_FPoint projToScreen(glm::vec2 proj) const;
    glm::vec2 screenToProj(SDL_Point p) const;

    SDL_FPoint coordsToScreen(Coord coords) const { return projToScreen(coordsToProj(coords)); }
    Coord screenToCoords(SDL_Point p) const { return projToCoords(screenToProj(p)); }

    void registerEvents(Event::EventManager& manager);

    void move(glm::vec2 v);

    SDL_Rect getScreenViewportRect() const { return {0, 0, width, height}; }

    float getZoom() const { return zoom; }

private:
    float zoom = 1.0f;
    glm::vec2 pos;

    int width, height;

};
