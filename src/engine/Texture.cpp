#include "Texture.hpp"

#include "../../lib/json.hpp"

#include <SDL_image.h>
#include <fstream>

#include "Utils.h"

bool Texture::createBlank(SDL_Renderer& rend, int w, int h, SDL_TextureAccess acc) {
    texture.reset(SDL_CreateTexture(&rend, SDL_PIXELFORMAT_RGBA8888, acc, w, h));
    if(!texture)
        return false;

    width = w;
    height = h;

    return true;
}

bool TextureManager::loadTexture(SDL_Renderer& renderer, const std::string& name, const std::filesystem::path& path) {
    textureMap[name] = std::move(Texture());

    if(!textureMap[name].loadFromFile(renderer, path))
        return false;
    return true;
}

bool Texture::loadFromFile(SDL_Renderer& renderer, const std::filesystem::path& path) {
    auto surface = IMG_Load(path.string().c_str());
    if(!surface) {
        writeError("Unable to load %s. SDL_Image error: %s", path.c_str(), IMG_GetError());
        return false;
    }

    texture.reset(SDL_CreateTextureFromSurface(&renderer, surface));
    SDL_FreeSurface(surface);
    if(!texture) {
        writeError("Unable to create texture %s. SDL error: %s", path.c_str(), SDL_GetError());
        return false;
    }

    SDL_QueryTexture(texture.get(), nullptr, nullptr, &width, &height);

    return true;
}

void TextureManager::loadTexture(const std::string& name, Texture&& texture) {
    textureMap[name] = std::forward<Texture>(texture);
}
