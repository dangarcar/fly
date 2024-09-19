#include "Renderer.hpp"

#include "Log.h"
#include "Gradient.h"

TextRenderer::TextRenderer() {
    for(int i=0; i<=MAX_FONT_SIZE; ++i)
        fonts.push_back(Font(FC_CreateFont(), &FC_FreeFont));
}

bool TextRenderer::start(SDL_Renderer& renderer) {
    bool correct = true;
    for(int i=1; i<=MAX_FONT_SIZE; ++i) { //Size 0 makes no sense
        correct &= FC_LoadFont(fonts[i].get(), &renderer, FONT_SRC, i, FC_MakeColor(0xFF, 0xFF, 0xFF, 0xFF), TTF_STYLE_NORMAL);
    }

    return correct;
}

Texture TextRenderer::renderToTexture(SDL_Renderer& renderer, const std::string& str) const {
    Texture text;
    auto rect = getTextBounds(str, MAX_FONT_SIZE);
    text.createBlank(renderer, rect.w, rect.h, SDL_TEXTUREACCESS_TARGET);
    
    text.setAsRenderTarget(renderer);
    render(renderer, str, 0, 0, MAX_FONT_SIZE, FC_MakeEffect(FC_ALIGN_LEFT, {1,1}, SDL_WHITE));
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
        writeError("Font could not be loaded!\n");
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
