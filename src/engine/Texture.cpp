#include "Texture.hpp"

#include "../../include/json.hpp"

#include <SDL_image.h>
#include <fstream>

#include "Log.hpp"

bool Texture::createBlank(SDL_Renderer& rend, int w, int h, SDL_TextureAccess acc) {
    texture.reset(SDL_CreateTexture(&rend, SDL_PIXELFORMAT_RGBA8888, acc, w, h));
    if(!texture)
        return false;

    width = w;
    height = h;

    return true;
}

void Texture::render(SDL_Renderer& renderer, int x, int y, SDL_Rect* clip) const {
	SDL_Rect renderQuad = { x, y, width, height };

	if(clip) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

    SDL_RenderCopy(&renderer, texture.get(), nullptr, &renderQuad);
}

void Texture::render(SDL_Renderer& renderer, int x, int y, float scale, float angle, SDL_BlendMode blendMode) const {
    int w = width * scale, h = height * scale;
    SDL_Rect renderQuad = { x, y, w, h };

    SDL_Point center = { 0, 0 };

    SDL_SetTextureBlendMode(texture.get(), blendMode); 
    SDL_RenderCopyEx(&renderer, texture.get(), nullptr, &renderQuad, angle, &center, SDL_FLIP_NONE);
}

bool TextureManager::loadTexture(SDL_Renderer& renderer, const std::string& name, const std::string& path) {
    textureMap[name] = std::move(Texture());
    
    if(!textureMap[name].loadFromFile(renderer, path))
        return false;
    return true;
}
bool Texture::loadFromFile(SDL_Renderer& renderer, const std::string& path) {
    auto surface = IMG_Load(path.c_str());
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

bool TextureManager::loadTexturePack(SDL_Renderer& renderer, const std::string& jsonPath) {
    using json = nlohmann::json;

    std::ifstream file(jsonPath);
    json data = json::parse(file);

    bool correct = true;
    for(auto& [k, v]: data.items()) {
        std::string path = v;
        correct &= this->loadTexture(renderer, k, path);
    }

    return correct;
}
