#pragma once

#include <SDL.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <filesystem>

#include "ShaderProgram.hpp"

struct Texture {
    int width = 0, height = 0;
    unsigned texture;
};

/*class Texture {
public:
    //Texture(): texture(nullptr, &SDL_DestroyTexture) {}
    Texture(std::filesystem::path path);

    //void setColorMod(SDL_Color color) const { SDL_SetTextureColorMod(texture.get(), color.r, color.g, color.b); }

    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    
};*/

class Renderer;

class TextureManager {
private:
    std::unordered_map<std::string, Texture> textureMap;

    inline static const std::filesystem::path VERTEX_SHADER_SRC = "./src/shaders/texture.vs";
    inline static const std::filesystem::path FRAGMENT_SHADER_SRC = "./src/shaders/texture.fs";
    unsigned VAO, VBO, EBO;
    ShaderProgram shader;
    int projectionLoc = -1;

public:
    TextureManager(): shader(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC) {}

    bool start();

    void loadTexture(const std::string& name, std::filesystem::path path);

    void render(const Renderer& r, const std::string& name, float x, float y, SDL_FRect* clip) const;
    void render(const Renderer& r, const std::string& name, float x, float y, float scale, float angle, bool centre) const;

    Texture getTexture(const std::string& name) const { return textureMap.at(name); }
};