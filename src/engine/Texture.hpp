#pragma once

#include <SDL.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <filesystem>

class Texture {
public:
    Texture(): texture(nullptr, &SDL_DestroyTexture) {}

    bool createBlank(SDL_Renderer& rend, int w, int h, SDL_TextureAccess acc);
    bool loadFromFile(SDL_Renderer& renderer, const std::filesystem::path& path);

    void setColorMod(SDL_Color color) const { SDL_SetTextureColorMod(texture.get(), color.r, color.g, color.b); }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    SDL_Texture* getTexture() const { return texture.get(); }

private:
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture;

    int width = 0, height = 0;
};

class TextureManager {
private:
    std::unordered_map<std::string, Texture> textureMap;

public:
    void loadTexture(const std::string& name, Texture&& texture);
    bool loadTexture(SDL_Renderer& renderer, const std::string& name, const std::filesystem::path& path);

    const Texture& getTexture(const std::string& name) const { return textureMap.at(name); }
};