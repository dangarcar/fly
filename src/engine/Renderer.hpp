#pragma once

#include <SDL.h>
#include <SDL_FontCache.h>

#include <vector>
#include <memory>

#include "Texture.hpp"

class TextRenderer {
private:
    static constexpr int CACHES_NUMBER = 10;
    static constexpr auto FONT_SRC = "./resources/ds_digital/DS-DIGIB.TTF";

public:
    TextRenderer() {
        for(int i=0; i<=CACHES_NUMBER; ++i)
            fonts.push_back(Font(FC_CreateFont(), &FC_FreeFont));
    }

    bool start(SDL_Renderer& renderer);
    Texture renderToTexture(SDL_Renderer& renderer, const std::string& str, int size) const;
    
    void render(SDL_Renderer& renderer, const std::string& text, int x, int y, float size, FC_Effect effect) const;

    SDL_Rect getTextBounds(const std::string& str, float size) const;

private:
    using Font = std::unique_ptr<FC_Font, decltype(&FC_FreeFont)>;
    std::vector<Font> fonts;
};

class Renderer {
private:
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;
    TextureManager textureManager;
    TextRenderer textRenderer;

protected:
    int width, height;

public:
    Renderer(int w, int h): renderer(nullptr, &SDL_DestroyRenderer), width(w), height(h) {}

    Renderer(Renderer&&) = default;
    Renderer& operator=(Renderer&& that) = default;

    bool start(SDL_Window& window);
    void renderText(const std::string& str, int x, int y, float scale, FC_AlignEnum align, SDL_Color color) const;

    void fillRect(SDL_Rect rect, SDL_Color color) const;
    
    void render(const Texture& tex, int x, int y, SDL_Rect* clip, SDL_BlendMode blendMode=SDL_BLENDMODE_BLEND) const;
    void renderF(const Texture& tex, float x, float y, float scale, float angle, bool centre=false, SDL_BlendMode blendMode=SDL_BLENDMODE_BLEND) const;

    SDL_Renderer& getSDL() const { return *renderer; }
    TextureManager& getTextureManager() { return textureManager; }
    const TextureManager& getTextureManager() const { return textureManager; }
    TextRenderer& getTextRenderer() { return textRenderer; }
    const TextRenderer& getTextRenderer() const { return textRenderer; }

    SDL_Rect getScreenViewportRect() const { return {0, 0, width, height}; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void clearScreen() const { 
        SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, SDL_ALPHA_OPAQUE); 
        SDL_RenderClear(renderer.get());
    }
    void presentScreen() const { 
        SDL_RenderPresent(renderer.get());
    }
};
