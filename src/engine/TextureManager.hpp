#pragma once

#include <SDL.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <filesystem>

#include "ShaderProgram.hpp"
#include "Gradient.h"

struct Texture {
    int width = 0, height = 0;
    unsigned texture;
};

class Renderer;

class TextureManager {
private:
    std::unordered_map<std::string, Texture> textureMap;

    inline static const std::filesystem::path VERTEX_SHADER_SRC = "./src/shaders/texture.vs";
    inline static const std::filesystem::path FRAGMENT_SHADER_SRC = "./src/shaders/texture.fs";
    unsigned VAO, VBO, EBO;
    ShaderProgram shader;
    int projectionLoc = -1, modColorLoc = -1, useTextureLocation = -1;

public:
    TextureManager(): shader(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC) {}

    bool start();

    void loadTexture(const std::string& name, std::filesystem::path path);

    void render(const Renderer& r, const std::string& name, float x, float y, SDL_FRect* clip, SDL_Color mod=SDL_WHITE) const;
    void render(const Renderer& r, const std::string& name, float x, float y, float scale, float angle, bool centre, SDL_Color mod=SDL_WHITE) const;
    void fillRect(const Renderer& r, SDL_Rect rect, SDL_Color color) const;

    Texture getTexture(const std::string& name) const { return textureMap.at(name); }

private:
    void renderPriv(const Renderer& r, const Texture* t, SDL_FRect rect, float rotation, SDL_Color color) const;
};