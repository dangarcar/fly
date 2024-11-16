#include "Renderer.hpp"

#include "Utils.h"

#include <glad/glad.h>
#include <SDL_opengl.h>

Renderer::Renderer(int w, int h, SDL_Window& window): width(w), height(h) {
    if(SDL_GL_LoadLibrary(nullptr) != 0) {
        writeError("GL Library could not be loaded! SDL_Error: %s\n", SDL_GetError());
    }

    const auto contextFlags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | (OPENGL_DEBUG ? SDL_GL_CONTEXT_DEBUG_FLAG : 0);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_MINOR_VERSION);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    this->context = SDL_GL_CreateContext(&window);
    if(!context) {
        writeError("Context could not be created! SDL_Error: %s\n", SDL_GetError());
    }

    SDL_GL_SetSwapInterval(0);

    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        writeError("GLAD could not be loaded! Error: %s\n", glad_glGetError());
    }

    writeLog("Vendor:   %s\n", glGetString(GL_VENDOR));
    writeLog("Renderer: %s\n", glGetString(GL_RENDERER));
    writeLog("Version:  %s\n", glGetString(GL_VERSION));
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_NEVER);
    glDisable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE); 

    glViewport(0, 0, width, height);

    textRenderer = TextRenderer(); //Make value
    if(!textRenderer->start()) {
        writeError("Fonts couldn't be loaded from the files\n");
    }

    textureManager = TextureManager();
    if(!textureManager->start()) {
        writeError("Couldn't create texture manager\n");
    } 
}

void Renderer::clearScreen() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::fillRect(SDL_Rect rect, SDL_Color color) const {
    textureManager->fillRect(*this, toFRect(rect), color, 0.0f);
}

void Renderer::fillRect(SDL_FRect rect, SDL_Color color, float rotation) const {
    textureManager->fillRect(*this, rect, color, rotation);
}

void Renderer::render(const std::string& tex, float x, float y, std::optional<SDL_FRect> clip, SDL_Color color) const {
    SDL_FRect* rect = nullptr;
    if(clip.has_value())
        rect = &clip.value();
    
    textureManager->render(*this, tex, x, y, rect, color);
}

void Renderer::renderExt(const std::string& tex, float x, float y, float scale, float angle, bool centre=false, SDL_Color color) const {
    textureManager->render(*this, tex, x, y, scale, angle, centre, color);
}

