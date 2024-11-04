#include "Renderer.hpp"

#include "Utils.h"
#include "Gradient.h"

#include <glad/glad.h>
#include <SDL_opengl.h>

bool Renderer::start(SDL_Window& window) {
    if(SDL_GL_LoadLibrary(nullptr) != 0) {
        writeError("GL Library could not be loaded! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    const auto contextFlags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | (OPENGL_DEBUG ? SDL_GL_CONTEXT_DEBUG_FLAG : 0);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_MINOR_VERSION);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    this->context = SDL_GL_CreateContext(&window);
    if(!context) {
        writeError("Context could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        writeError("GLAD could not be loaded! Error: %s\n", glad_glGetError());
        return false;
    }

    writeLog("Vendor:   %s\n", glGetString(GL_VENDOR));
    writeLog("Renderer: %s\n", glGetString(GL_RENDERER));
    writeLog("Version:  %s\n", glGetString(GL_VERSION));
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH);

    glViewport(0, 0, width, height);

    textRenderer = TextRenderer(); //Make value
    if(!textRenderer.value().start()) {
        writeError("Fonts couldn't be loaded from the files");
        return false;
    }

    return true;
}

void Renderer::clearScreen() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::fillRect(SDL_Rect rect, SDL_Color color) const { //TODO:
    /*SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer.get(), color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer.get(), &rect);*/
}

void Renderer::render(const Texture& tex, int x, int y, SDL_Rect* clip, SDL_BlendMode blendMode) const { //TODO:
	/*SDL_Rect renderQuad = { x, y, tex.getWidth(), tex.getHeight() };

	if(clip) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

    SDL_SetTextureBlendMode(tex.getTexture(), blendMode);
    SDL_RenderCopy(renderer.get(), tex.getTexture(), nullptr, &renderQuad);*/
}

void Renderer::renderF(const Texture& tex, float x, float y, float scale, float angle, bool centre, SDL_BlendMode blendMode) const { //TODO:
    /*float w = tex.getWidth() * scale, h = tex.getHeight() * scale;
    SDL_FRect renderQuad = { x, y, w, h };
    if(centre)
        renderQuad = { x - w/2 , y - h/2 , w, h };

    SDL_FPoint centrePoint = { 0, 0 };        

    SDL_SetTextureBlendMode(tex.getTexture(), blendMode); 
    SDL_RenderCopyExF(renderer.get(), tex.getTexture(), nullptr, &renderQuad, angle, centre? nullptr:&centrePoint, SDL_FLIP_NONE);*/
}

