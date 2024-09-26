#pragma once

#include <SDL.h>
#include <string>
#include <unordered_map>
#include <memory>

class Texture {
public:
    Texture(): texture(nullptr, &SDL_DestroyTexture) {}

    bool createBlank(SDL_Renderer& rend, int w, int h, SDL_TextureAccess acc);
    bool loadFromFile(SDL_Renderer& renderer, const std::string& path);
    bool loadSVG(SDL_Renderer& renderer, const std::string& svg);

    void render(SDL_Renderer& renderer, int x, int y, SDL_Rect* clip, SDL_BlendMode blendMode=SDL_BLENDMODE_BLEND) const;
    void render(SDL_Renderer& renderer, int x, int y, float scale, float angle, SDL_BlendMode blendMode=SDL_BLENDMODE_BLEND) const;
    void renderCenter(SDL_Renderer& renderer, float x, float y, float scale, float angle) const;
    void setColorMod(SDL_Color color) const { SDL_SetTextureColorMod(texture.get(), color.r, color.g, color.b); }

    void setAsRenderTarget(SDL_Renderer& renderer) { 
        SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND); 
        SDL_SetRenderTarget(&renderer, texture.get()); 
    }

    void applyMask(SDL_Renderer& renderer, const Texture& mask);

    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture;

    int width = 0, height = 0;
};

class TextureManager {
private:
    std::unordered_map<std::string, Texture> textureMap;

public:
    void loadTexture(const std::string& name, Texture&& texture);
    bool loadTexture(SDL_Renderer& renderer, const std::string& name, const std::string& path);
    //bool loadTexturePack(SDL_Renderer& renderer, const std::string& jsonPath);

    const Texture& getTexture(const std::string& name) const { return textureMap.at(name); }
};