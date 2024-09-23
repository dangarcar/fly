#include "Renderer.hpp"

#include "Utils.h"
#include "Gradient.h"

bool TextRenderer::start(SDL_Renderer& renderer) {
    bool correct = true;
    for(int i=MIN_PRELOADED_FONT_SIZE; i<=MAX_PRELOADED_FONT_SIZE; ++i) {
        correct &= loadFontSize(i, renderer);
    }
    return correct;
}

Texture TextRenderer::renderToTexture(SDL_Renderer& renderer, const std::string& str, int size) const {
    Texture text;
    auto rect = getTextBounds(str, size);
    text.createBlank(renderer, rect.w, rect.h, SDL_TEXTUREACCESS_TARGET);
    
    text.setAsRenderTarget(renderer);
    render(renderer, str, 0, 0, size, FC_MakeEffect(FC_ALIGN_LEFT, {1,1}, SDL_WHITE));
    SDL_SetRenderTarget(&renderer, nullptr);
    
    return text;
}

bool Renderer::start(SDL_Window& window) {
    renderer.reset(SDL_CreateRenderer(&window, -1, SDL_RENDERER_ACCELERATED));
    
    if(!renderer) {
        writeError("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    
    if(!textRenderer.start(*renderer)) {
        writeError("Fonts couldn't be loaded from the files");
        return false;
    }

    if(!textureManager.loadTexturePack(*renderer, DEFAULT_TEXTURE_PACK)) {
        writeError("Textures couldn't be loaded from the files");
        return false;
    }

    return true;
}

void Renderer::renderText(const std::string& str, int x, int y, float scale, FC_AlignEnum align, SDL_Color color) const {
    FC_Effect effect;
    effect.color = color;
    effect.alignment = align;
    effect.scale = {1, 1};
    textRenderer.render(*renderer, str, x, y, scale, effect); 
}
