#include "TextureManager.hpp"

#include <json/json.hpp>
#include <fstream>

#include "Utils.h"
#include "Renderer.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION 1
#include <stb/stb_image.h>

bool TextureManager::start() {
    float vertices[] = {
         1.0f,  1.0f,   1.0f, 1.0f,   // top right
         1.0f, -1.0f,   1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f,   0.0f, 0.0f,   // bottom left
        -1.0f,  1.0f,   0.0f, 1.0f    // top left 
    };
    unsigned indices[] = {
        0, 1, 3, 1, 2, 3
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    projectionLoc = glGetUniformLocation(shader.getId(), "projection");
    modColorLoc = glGetUniformLocation(shader.getId(), "modColor");
    useTextureLocation = glGetUniformLocation(shader.getId(), "useTexture");

    return projectionLoc != -1 && modColorLoc != -1 && useTextureLocation != -1;
}

void TextureManager::loadTexture(const std::string& name, std::filesystem::path path) {
    Texture t;

    glGenTextures(1, &t.texture);
    glBindTexture(GL_TEXTURE_2D, t.texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path.string().c_str(), &t.width, &t.height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t.width, t.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        writeError("Failed to load texture\n");
    }

    stbi_image_free(data);

    textureMap[name] = t;
}

void TextureManager::render(const Renderer& r, const std::string& name, float x, float y, SDL_FRect* clip, SDL_Color mod) const {    
    auto& t = textureMap.at(name);
    float w = t.width, h = t.height;
    if(clip) {
        w = clip->w;
        h = clip->h;
    }

    this->renderPriv(r, &t, {x, y, w, h}, 0.0f, mod);
}

void TextureManager::render(const Renderer& r, const std::string& name, float x, float y, float scale, float angle, bool centre, SDL_Color mod) const {
    auto& t = textureMap.at(name);
    float w = scale;
    float h = scale;

    if(centre) {
        x -= w/2;
        y -= h/2;
    }

    this->renderPriv(r, &t, {x, y, w, h}, angle, mod);
}

void TextureManager::fillRect(const Renderer& r, SDL_Rect rect, SDL_Color color) const {
    this->renderPriv(r, nullptr, toFRect(rect), 0.0f, color);
}

void TextureManager::renderPriv(const Renderer& r, const Texture* t, SDL_FRect rect, float angle, SDL_Color color) const {
    shader.use();

    auto projection = glm::mat4(1.0f);
    projection = glm::translate(projection, glm::vec3(-1, 1, 0)); //Change center to top-left
    projection = glm::scale(projection, glm::vec3(rect.w / r.getWidth(), rect.h / r.getHeight(), 1));
    projection = glm::translate(projection, glm::vec3(1 + 2*rect.x/rect.w, -(1 + 2*rect.y/rect.h), 0));
    projection = glm::rotate(projection, glm::radians(-angle), glm::vec3(0, 0, 1));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform4f(modColorLoc, color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f);
    glUniform1i(useTextureLocation, t != nullptr);

    if(t) glBindTexture(GL_TEXTURE_2D, t->texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
