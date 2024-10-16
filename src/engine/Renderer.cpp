#include "Renderer.hpp"

#include "Utils.h"
#include "Gradient.h"

SDL_Rect TextRenderer::getTextBounds(const std::string& str, float size) const {
    int i = std::clamp(0, CACHES_NUMBER, int(std::ceil(std::log2(size))));
    auto rect = FC_GetBounds(fonts[i].get(), 0, 0, FC_ALIGN_LEFT, {1,1}, str.c_str());
    float scale = size / (1 << i);
    
    rect.w *= scale;
    rect.h *= scale;

    return rect;
}

bool TextRenderer::start(SDL_Renderer& renderer) {
    bool correct = true;
    for(int i=0; i<=CACHES_NUMBER; ++i) {
        correct &= FC_LoadFont(fonts[i].get(), &renderer, FONT_SRC, 1 << i, SDL_WHITE, TTF_STYLE_NORMAL);
    }
    return correct;
}

void TextRenderer::render(SDL_Renderer& renderer, const std::string& text, int x, int y, float size, FC_Effect effect) const {
    int i = std::clamp(0, CACHES_NUMBER, int(std::ceil(std::log2(size))));
    effect.scale.x = effect.scale.y = size / (1 << i);
    FC_DrawEffect(fonts[i].get(), &renderer, x, y, effect, text.c_str());
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

bool Renderer::start() {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    
    if(!textRenderer.start(renderer)) {
        writeError("Fonts couldn't be loaded from the files");
        return false;
    }

    return true;
}

void Renderer::renderText(const std::string& str, int x, int y, float scale, FC_AlignEnum align, SDL_Color color) const {
    FC_Effect effect;
    effect.color = color;
    effect.alignment = align;
    effect.scale = {1, 1};
    textRenderer.render(renderer, str, x, y, scale, effect); 
}
