#pragma once

#include <SDL.h>
#include <SDL_FontCache.h>

#include <unordered_map>
#include <memory>

#include "Texture.hpp"

class TextRenderer {
private:
    static constexpr int MAX_PRELOADED_FONT_SIZE = 64;
    static constexpr int MIN_PRELOADED_FONT_SIZE = 4;
    static constexpr auto FONT_SRC = "./resources/ds_digital/DS-DIGIB.TTF";

public:
    bool start(SDL_Renderer& renderer);
    Texture renderToTexture(SDL_Renderer& renderer, const std::string& str, int size) const;
    
    void render(SDL_Renderer& renderer, const std::string& text, int x, int y, int size, FC_Effect effect) const {
        FC_DrawEffect(fonts.at(size).get(), &renderer, x, y, effect, text.c_str());
    }

    SDL_Rect getTextBounds(const std::string& str, int size) const {
        return FC_GetBounds(fonts.at(size).get(), 0, 0, FC_ALIGN_LEFT, {1,1}, str.c_str());
    }

    bool loadFontSize(int size, SDL_Renderer& renderer) {
        if(!fonts.contains(size)) {
            fonts.insert(std::make_pair(size, Font(FC_CreateFont(), &FC_FreeFont)));
            return FC_LoadFont(fonts.at(size).get(), &renderer, FONT_SRC, size, FC_MakeColor(0xFF, 0xFF, 0xFF, 0xFF), TTF_STYLE_NORMAL);
        }

        return true;
    }

private:
    using Font = std::unique_ptr<FC_Font, decltype(&FC_FreeFont)>;
    std::unordered_map<int, Font> fonts;
};

class Renderer {
private:
    static constexpr auto DEFAULT_TEXTURE_PACK = "./resources/textures.json";

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
    TextRenderer& getTextRenderer() { return textRenderer; }
    const TextRenderer& getTextRenderer() const { return textRenderer; }

    void clearScreen() const { SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, SDL_ALPHA_OPAQUE); SDL_RenderClear(renderer.get()); }
    void presentScreen() const { SDL_RenderPresent(renderer.get()); }
};
