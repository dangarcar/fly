#pragma once

#include "ShaderProgram.hpp"
#include <SDL.h>
#include <vector>
#include <filesystem>
#include <cassert>

struct Character {
    char c = 0;
    float width = 0; //Assuming h = 1
    unsigned VAO, VBO, EBO;
    int elements = 0;
};

enum class Aligment {
    LEFT, CENTER, RIGHT 
};

class Renderer;

class TextRenderer {
private:
    inline static const std::filesystem::path FONT_FILE = "./resources/font.fontobj";
    inline static const std::filesystem::path VERTEX_SHADER_SRC = "./src/shaders/text.vs";
    inline static const std::filesystem::path FRAGMENT_SHADER_SRC = "./src/shaders/text.fs";

public:
    TextRenderer(): shader(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC) {}

    bool start();

    void render(const Renderer& r, const std::string& s, int x, int y, float size, Aligment align, SDL_Color color) const;

    //Cannot use newline character nor aligments in rotated mode
    void renderRotated(const Renderer& r, const std::string& s, int x, int y, float size, float angle, SDL_Color color) const;

    SDL_FRect getBounds(const std::string& s, float size) const;

private:
    std::string getLower(const std::string& s) const;

    ShaderProgram shader;
    int projectionLoc = -1, viewLoc = -1, modelLoc = -1, colorLoc = -1;

    Character chars[256];
};
