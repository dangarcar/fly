#pragma once

#define GLM_ENABLE_EXPERIMENTAL 1

#include <SDL.h>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>

#include "engine/Window.hpp"

const float LATITUDE_LIMIT = glm::degrees(2.0 * atan(exp(M_PI)) - M_PI_2);

constexpr SDL_Color SEA_COLOR = {0x03, 0x19, 0x40, SDL_ALPHA_OPAQUE};
//constexpr SDL_Color SEA_COLOR = {0x40, 0xD3, 0xF0, SDL_ALPHA_OPAQUE};

struct Coord {
    float lon, lat;
};

class Camera {
private:
    static constexpr float MAX_ZOOM = 24.0f;
    static constexpr float ZOOM_SPEED = 0.15f;

    static constexpr Coord INITIAL_POS = {0, 30};

public:
    Camera(int w, int h): width(w), height(h) {
        pos = coordsToProj(INITIAL_POS);
    }

    inline glm::vec2 coordsToProj(Coord coords) const;
    inline Coord projToCoords(glm::vec2 proj) const;

    inline SDL_FPoint projToScreen(glm::vec2 proj) const;
    inline glm::vec2 screenToProj(SDL_Point p) const;

    SDL_FPoint coordsToScreen(Coord coords) const { return projToScreen(coordsToProj(coords)); }
    Coord screenToCoords(SDL_Point p) const { return projToCoords(screenToProj(p)); }

    void handleEvents(const Event& event);

    void moveCamera(glm::vec2 v);

    SDL_Rect getScreenViewportRect() const { return {0, 0, width, height}; }

    float getZoom() const { return zoom; }

private:
    float zoom = 1.0f;
    glm::vec2 pos;

    int width, height;
};

glm::vec2 Camera::coordsToProj(Coord coords) const {
    glm::vec2 proj;
    const double M_1_2PI = 1.0 / (2.0 * M_PI);
    proj.x = width * M_1_2PI * (M_PI + glm::radians(coords.lon));
    proj.y = width * M_1_2PI * (M_PI - log(tan(M_PI_4 + glm::radians(coords.lat)/2.0)));
    return proj;
}

Coord Camera::projToCoords(glm::vec2 proj) const {
    Coord c;
    c.lon = glm::degrees(2.0 * M_PI * proj.x / width - M_PI); 
    c.lat = glm::degrees(2.0 * (atan(exp(M_PI - 2.0 * M_PI * proj.y / width)) - M_PI_4));
    return c;
}

SDL_FPoint Camera::projToScreen(glm::vec2 proj) const {
    SDL_FPoint p;
    p.x = (proj.x - pos.x + width/2) * zoom; 
    p.y = (proj.y - pos.y + height/2) * zoom;
    return p;
}

glm::vec2 Camera::screenToProj(SDL_Point p) const {
    glm::vec2 proj;
    proj.x = p.x / zoom + pos.x - width/2; 
    proj.y = p.y / zoom + pos.y - height/2;
    return proj;
}

void Camera::handleEvents(const Event& event) {
    if(auto wheelevent = std::get_if<MouseWheelEvent>(&event)) {
        float newZoom;
        if(wheelevent->direction < 0)
            newZoom = std::max(1.0f, zoom - zoom*ZOOM_SPEED);
        else
            newZoom = std::min(MAX_ZOOM, zoom + zoom*ZOOM_SPEED);
        auto dz = (zoom - newZoom) / (zoom * newZoom);
        zoom = newZoom;
                
        moveCamera(glm::vec2(wheelevent->mousePos.x, wheelevent->mousePos.y) * dz);
    }

    else if(auto dragevent = std::get_if<DragEvent>(&event)) {
        auto p1 = screenToProj(dragevent->oldPos);
        auto p2 = screenToProj(dragevent->newPos);
        moveCamera(p2 - p1);
    }

    else if(auto resizedevent = std::get_if<WindowResizedEvent>(&event)) {
        auto normPos = pos / float(width);
        width = resizedevent->width;
        height = resizedevent->height;
        pos = normPos * float(width);
    }

    else if(auto keyevent = std::get_if<KeyPressedEvent>(&event)) {
        if(keyevent->keycode == SDLK_0) //DEBUG:
            zoom = 10.0f;
    }
}

void Camera::moveCamera(glm::vec2 v) {
    pos = {pos.x - v.x, pos.y - v.y};

    auto top = screenToProj({0, 0});
    auto bottom = screenToProj({width, height});

    if(top.y < 0)
        pos.y += -top.y;
    else if(bottom.y > width)
        pos.y += width - bottom.y;

    if(top.x <= 0)
        pos.x += -top.x;
    else if(bottom.x >= width)
        pos.x += width - bottom.x;
}