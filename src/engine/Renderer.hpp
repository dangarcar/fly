#pragma once

#include <SDL.h>

#include <optional>
#include <vector>
#include <memory>

#include "Texture.hpp"
#include "TextRenderer.hpp"

constexpr int OPENGL_MAJOR_VERSION = 4;
constexpr int OPENGL_MINOR_VERSION = 3;
constexpr bool OPENGL_DEBUG = false;

class Renderer {
private:
    //std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;
    SDL_GLContext context;
    //TextureManager textureManager;
    std::optional<TextRenderer> textRenderer;

protected:
    int width, height;

public:
    Renderer(int w, int h): width(w), height(h) {}

    Renderer(Renderer&&) = default;
    Renderer& operator=(Renderer&& that) = default;

    bool start(SDL_Window& window);
    
    void renderText(const std::string& str, int x, int y, float size, Aligment align, SDL_Color color) const {
        textRenderer.value().render(*this, str, x, y, size, align, color);
    }
    void renderTextRotated(const std::string& str, int x, int y, float size, float angle, SDL_Color color) const {
        textRenderer.value().renderRotated(*this, str, x, y, size, angle, color);
    }
    SDL_FRect getTextBounds(const std::string& str, float size) const {
        return textRenderer.value().getBounds(str, size);
    }

    void fillRect(SDL_Rect rect, SDL_Color color) const;
    
    void render(const Texture& tex, int x, int y, SDL_Rect* clip, SDL_BlendMode blendMode=SDL_BLENDMODE_BLEND) const;
    void renderF(const Texture& tex, float x, float y, float scale, float angle, bool centre=false, SDL_BlendMode blendMode=SDL_BLENDMODE_BLEND) const;

    /*TextureManager& getTextureManager() { return textureManager; }
    const TextureManager& getTextureManager() const { return textureManager; }*/

    void clearScreen();

    SDL_Rect getScreenViewportRect() const { return {0, 0, width, height}; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};
