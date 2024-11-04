#pragma once

#include <glm/glm.hpp>

#include "../engine/Renderer.hpp"
#include "Types.h"
#include "../engine/InputEvent.h"
#include "../engine/Serializable.h"

struct CameraSave {
    float zoom;
    glm::vec2 pos;
};

class Camera: Serializable<CameraSave> {
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

    CameraSave serialize() const override;
    void deserialize(const CameraSave& save) override;

    //SERVE AS RENDERER MEDIATOR: probably bs
    const Renderer& getRenderer() const { return renderer; }

    SDL_Rect getScreenViewportRect() const { return {0, 0, width, height}; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void renderText(const std::string& str, int x, int y, float size, Aligment align, SDL_Color color) const { 
        renderer.renderText(str, x, y, size, align, color);
    }
    void renderTextRotated(const std::string& str, int x, int y, float size, float angle, SDL_Color color) const {
        renderer.renderTextRotated(str, x, y, size, angle, color);
    }
    SDL_FRect getTextBounds(const std::string& str, float size) const {
        return renderer.getTextBounds(str, size);
    }

    void fillRect(SDL_Rect rect, SDL_Color color) const { renderer.fillRect(rect, color); }

    void render(const Texture& tex, int x, int y, SDL_Rect* clip, SDL_BlendMode blendMode=SDL_BLENDMODE_BLEND) const {
        renderer.render(tex, x, y, clip, blendMode);
    }
    void renderF(const Texture& tex, float x, float y, float scale, float angle, bool centre=false, SDL_BlendMode blendMode=SDL_BLENDMODE_BLEND) const {
        renderer.renderF(tex, x, y, scale, angle, centre, blendMode);
    }

    //SDL_Renderer& getSDL() const { return renderer.getSDL(); }
    TextureManager& getTextureManager() { return renderer.getTextureManager(); }
    const TextureManager& getTextureManager() const { return renderer.getTextureManager(); }

private:
    Renderer& renderer;
    
    float zoom = 1.0f;
    glm::vec2 pos;
    int width, height;

};
