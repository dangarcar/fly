#pragma once

#include "Log.hpp"

#include "SDL_FontCache.h"
#include "Texture.hpp"
#include "Gradient.hpp"

#include <format>
#include <memory>
#include <vector>

#define FONT_SRC "./resources/ds_digital/DS-DIGIB.TTF"

constexpr int MAX_FONT_SIZE = 128;

class TextRenderer {
public:
    TextRenderer() {
        for(int i=0; i<=MAX_FONT_SIZE; ++i)
            fonts.push_back(Font(FC_CreateFont(), &FC_FreeFont));
    }

    bool start(SDL_Renderer& renderer) {
        bool correct = true;
        for(int i=1; i<=MAX_FONT_SIZE; ++i) { //Size 0 makes no sense
            correct &= FC_LoadFont(fonts[i].get(), &renderer, FONT_SRC, i, FC_MakeColor(0xFF, 0xFF, 0xFF, 0xFF), TTF_STYLE_NORMAL);
        }

        return correct;
    }
    
    void render(SDL_Renderer& renderer, const std::string& text, int x, int y, int size, FC_Effect effect) const {
        FC_DrawEffect(fonts[size].get(), &renderer, x, y, effect, text.c_str());
    }

    SDL_Rect getTextBounds(const std::string& str, int size) const {
        return FC_GetBounds(fonts[size].get(), 0, 0, FC_ALIGN_LEFT, {1,1}, str.c_str());
    }

    Texture renderToTexture(SDL_Renderer& renderer, const std::string& str) const {
        Texture text;
        auto rect = getTextBounds(str, MAX_FONT_SIZE);

        text.createBlank(renderer, rect.w, rect.h, SDL_TEXTUREACCESS_TARGET);

        text.setAsRenderTarget(renderer);
        render(renderer, str, 0, 0, MAX_FONT_SIZE, FC_MakeEffect(FC_ALIGN_LEFT, {1,1}, SDL_WHITE));
        SDL_SetRenderTarget(&renderer, nullptr);

        return text;
    }

private:
    using Font = std::unique_ptr<FC_Font, decltype(&FC_FreeFont)>;
    std::vector<Font> fonts;
};