#include "Texture.hpp"

#include "../../include/json.hpp"

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

void Texture::render(SDL_Renderer& renderer, int x, int y, SDL_Rect* clip, SDL_BlendMode blendMode) const {
	SDL_Rect renderQuad = { x, y, width, height };

	if(clip) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

    SDL_SetTextureBlendMode(texture.get(), blendMode);
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

bool Texture::loadSVG(SDL_Renderer& renderer, const std::string& svg) {
    auto rw = SDL_RWFromConstMem(svg.c_str(), svg.size());
    auto surface = IMG_Load_RW(rw, 1);
    
    texture.reset(SDL_CreateTextureFromSurface(&renderer, surface));
    SDL_FreeSurface(surface);
    if(!texture) {
        writeError("Unable to create texture %s. SDL error: %s", svg.c_str(), SDL_GetError());
        return false;
    }

    SDL_QueryTexture(texture.get(), nullptr, nullptr, &width, &height);

    return true;
}

void Texture::applyMask(SDL_Renderer& renderer, const Texture& mask) {    
    Texture canvas;
    canvas.createBlank(renderer, this->width, this->height, SDL_TEXTUREACCESS_TARGET);
    canvas.setAsRenderTarget(renderer);
    
    mask.render(renderer, 0, 0, nullptr);
    
    SDL_SetTextureBlendMode(this->texture.get(), SDL_BLENDMODE_MUL);
    SDL_RenderCopy(&renderer, this->texture.get(), nullptr, nullptr);

    SDL_SetRenderTarget(&renderer, nullptr);
    *this = std::move(canvas);
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

void TextureManager::loadTexture(const std::string& name, Texture&& texture) {
    textureMap[name] = std::forward<Texture>(texture);
}
