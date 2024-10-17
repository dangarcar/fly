#pragma once

#include <glm/glm.hpp>

#include "../engine/Renderer.hpp"
#include "Types.h"
#include "../engine/InputEvent.h"

class Camera {
public:
    static constexpr float MAX_ZOOM = 40.0f;

private:
    static constexpr float ZOOM_SPEED = 0.15f;
    static constexpr Coord INITIAL_POS = {0, 33};

public:
    Camera(Renderer& renderer): renderer(renderer), width(renderer.getWidth()), height(renderer.getHeight()) {
        pos = coordsToProj(INITIAL_POS);
    }

    glm::vec2 coordsToProj(Coord coords) const;
    Coord projToCoords(glm::vec2 proj) const;

    SDL_FPoint projToScreen(glm::vec2 proj) const;
    glm::vec2 screenToProj(SDL_Point p) const;

    SDL_FPoint coordsToScreen(Coord coords) const { return projToScreen(coordsToProj(coords)); }
    Coord screenToCoords(SDL_Point p) const { return projToCoords(screenToProj(p)); }

    void handleInput(const InputEvent& event);
    void move(glm::vec2 v);

    float getZoom() const { return zoom; }
    void setZoom(float z) { 
        assert(z >= 1 && z <= MAX_ZOOM);
        move(glm::vec2(width/2, height/2) * ((zoom - z) / (zoom * z))); 
        zoom = z;
    }

    //SERVE AS RENDERER MEDIATOR: probably bs

    const Renderer& getRenderer() const { return renderer; }

    SDL_Rect getScreenViewportRect() const { return {0, 0, width, height}; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void renderText(const std::string& str, int x, int y, float scale, FC_AlignEnum align, SDL_Color color) const { 
        renderer.renderText(str, x, y, scale, align, color);
    }

    SDL_Renderer& getSDL() const { return renderer.getSDL(); }
    TextureManager& getTextureManager() { return renderer.getTextureManager(); }
    const TextureManager& getTextureManager() const { return renderer.getTextureManager(); }
    TextRenderer& getTextRenderer() { return renderer.getTextRenderer(); }
    const TextRenderer& getTextRenderer() const { return renderer.getTextRenderer(); }

private:
    Renderer& renderer;
    
    float zoom = 1.0f;
    glm::vec2 pos;
    int width, height;

};
