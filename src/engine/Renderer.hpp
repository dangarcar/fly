#pragma once

#include <SDL.h>
#include <SDL_FontCache.h>

#include <vector>
#include <memory>

#include "Texture.hpp"

#define DEFAULT_TEXTURE_PACK "./resources/textures.json"

#define FONT_SRC "./resources/ds_digital/DS-DIGIB.TTF"

constexpr int MAX_FONT_SIZE = 128;

class TextRenderer {
public:
    TextRenderer();

    bool start(SDL_Renderer& renderer);
    Texture renderToTexture(SDL_Renderer& renderer, const std::string& str) const;
    
    void render(SDL_Renderer& renderer, const std::string& text, int x, int y, int size, FC_Effect effect) const {
        FC_DrawEffect(fonts[size].get(), &renderer, x, y, effect, text.c_str());
    }

    SDL_Rect getTextBounds(const std::string& str, int size) const {
        return FC_GetBounds(fonts[size].get(), 0, 0, FC_ALIGN_LEFT, {1,1}, str.c_str());
    }

private:
    using Font = std::unique_ptr<FC_Font, decltype(&FC_FreeFont)>;
    std::vector<Font> fonts;
};

class Renderer {
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;
    TextureManager textureManager;
    TextRenderer textRenderer;

public:
    Renderer(): renderer(nullptr, &SDL_DestroyRenderer) {}

    bool start(SDL_Window& window);
    void renderText(const std::string& str, int x, int y, float scale, FC_AlignEnum align, SDL_Color color) const;

    SDL_Renderer* getSDL() const { return renderer.get(); }
    TextureManager& getTextureManager() { return textureManager; }
    const TextureManager& getTextureManager() const { return textureManager; }
    const TextRenderer& getTextRenderer() const { return textRenderer; }

    void clearScreen() const { SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, SDL_ALPHA_OPAQUE); SDL_RenderClear(renderer.get()); }
    void presentScreen() const { SDL_RenderPresent(renderer.get()); }
};
