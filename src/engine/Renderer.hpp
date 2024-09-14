#pragma once

#include <SDL.h>
#include "TextRenderer.hpp"
#include "Texture.hpp"

#include <memory>

#define DEFAULT_TEXTURE_PACK "./resources/textures.json"

class Renderer {
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;
    TextureManager textureManager;
    TextRenderer textRenderer;

public:
    Renderer(): renderer(nullptr, &SDL_DestroyRenderer) {}

    bool start(SDL_Window& window);
    void renderText(const std::string& str, int x, int y, float scale, FC_AlignEnum align, SDL_Color color) const {
        FC_Effect effect;
        effect.color = color;
        effect.alignment = align;
        effect.scale = {1, 1};
        textRenderer.render(*renderer, str, x, y, scale, effect); 
    }

    SDL_Renderer* getSDL() const { return renderer.get(); }
    const TextureManager& getTextureManager() const { return textureManager; }
    const TextRenderer& getTextRenderer() const { return textRenderer; }

    void clearScreen() const { SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, SDL_ALPHA_OPAQUE); SDL_RenderClear(renderer.get()); }
    void presentScreen() const { SDL_RenderPresent(renderer.get()); }
};

bool Renderer::start(SDL_Window& window) {
    renderer.reset(SDL_CreateRenderer(&window, -1, SDL_RENDERER_ACCELERATED));
    
    if(!renderer) {
        writeError("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    
    if(!textRenderer.start(*renderer)) {
        writeError("Font could not be loaded!\n");
        return false;
    }
    
    if(!textureManager.loadTexturePack(*renderer, DEFAULT_TEXTURE_PACK)) {
        writeError("Textures couldn't be loaded from the files");
        return false;
    }

    return true;
}
