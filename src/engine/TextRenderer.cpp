#include "TextRenderer.hpp"

#include "Renderer.hpp"
#include "Utils.h"
#include <fstream>
#include <sstream>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <glad/glad.h>

bool TextRenderer::start() {
    std::ifstream file(FONT_FILE);
    
    std::vector<float> vertices[256];
    std::vector<int> indices[256];

    std::string buf;
    char current = 0;
    while(getline(file, buf)) {
        if(buf.empty())
            continue;
        
        if(buf[0] == 'o') {
            current = buf[2];
            chars[size_t(current)].c = buf[2];
            chars[size_t(current)].width = std::strtof(buf.c_str() + 4, nullptr);
        } else if(buf[0] == 'v') {
            std::stringstream ss(buf.c_str() + 2);
            float v1, v2;
            ss >> v1 >> v2;
            vertices[size_t(current)].push_back(v1);
            vertices[size_t(current)].push_back(v2);
        } else if(buf[0] == 'f') {
            std::stringstream ss(buf.c_str() + 2);
            int f1, f2, f3;
            ss >> f1 >> f2 >> f3;
            indices[size_t(current)].push_back(f1);
            indices[size_t(current)].push_back(f2);
            indices[size_t(current)].push_back(f3);
        } else {
            writeError("Couldn't read letter");
            return false;
        }
    }
    
    for(int i=1; i<256; ++i) {
        if(chars[i].c == 0)
            continue;

        glGenVertexArrays(1, &chars[i].VAO);
        glBindVertexArray(chars[i].VAO);

        glGenBuffers(1, &chars[i].VBO);
        glBindBuffer(GL_ARRAY_BUFFER, chars[i].VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices[i].size()*sizeof(float), &vertices[i][0], GL_STATIC_DRAW);

        glGenBuffers(1, &chars[i].EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chars[i].EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size()*sizeof(int), &indices[i][0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        chars[i].elements = indices[i].size();
    }

    projectionLoc = glGetUniformLocation(shader.getId(), "projection");
    viewLoc = glGetUniformLocation(shader.getId(), "view");
    modelLoc = glGetUniformLocation(shader.getId(), "model");
    colorLoc = glGetUniformLocation(shader.getId(), "color");
    return projectionLoc != -1 && viewLoc != -1 && modelLoc != -1 && colorLoc != -1;
}

void TextRenderer::render(const Renderer& r, const std::string& str, int x, int y, float size, Aligment align, SDL_Color color) const {
    shader.use();
    
    glUniform4f(colorLoc, color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);

    auto projection = glm::ortho(0.0f, (float)r.getWidth(), (float)r.getHeight(), 0.0f);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(size, size, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    std::stringstream ss(getLower(str));
    std::string s;
    while(std::getline(ss, s)) {
        float dx = 0.0f;
        switch (align) {
            case Aligment::LEFT: break; //NOTHING
            case Aligment::CENTER: 
                dx -= getBounds(s, size).w / 2; 
                break;
            case Aligment::RIGHT: 
                dx -= getBounds(s, size).w; 
                break;
        }

        for(char i: s) {
            auto view = glm::mat4(1.0f);
            view = glm::translate(view, glm::vec3(x + dx, y, 0.0f));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            auto& ch = chars[size_t(i)];
            assert(ch.width != 0.0f);
            dx += ch.width * size;

            glBindVertexArray(ch.VAO);
            glDrawElements(GL_TRIANGLES, ch.elements, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        y += size;
    }
}

void TextRenderer::renderRotated(const Renderer& r, const std::string& in, int x, int y, float size, float angle, SDL_Color color) const {
    auto s = getLower(in);
    shader.use();
    
    glUniform4f(colorLoc, color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);

    auto projection = glm::ortho(0.0f, (float)r.getWidth(), (float)r.getHeight(), 0.0f);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(size, size, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::vec2 dv = {0, 0};
    for(char i: s) {
        auto view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(x + dv.x, y + dv.y, 0.0f));
        view = glm::rotate(view, glm::radians(angle), glm::vec3(0, 0, 1.0f));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        auto& ch = chars[size_t(i)];
        assert(ch.width != 0.0f);
        dv += glm::rotate(glm::vec2(ch.width * size, 0.0f), glm::radians(angle));

        glBindVertexArray(ch.VAO);
        glDrawElements(GL_TRIANGLES, ch.elements, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

SDL_FRect TextRenderer::getBounds(const std::string& s, float size) const {
    SDL_FRect rect = { 0.0f, 0.0f, 0.0f, size };

    float dx = 0.0f;
    for(char i: getLower(s)) {
        if(i == '\n') {
            rect.h += size;
            dx = 0;
        } else {
            dx += chars[size_t(i)].width * size;
            rect.w = std::max(dx, rect.w);
        }
    }

    return rect;
}

std::string TextRenderer::getLower(const std::string& in) const {
    std::string s = in;
    std::transform(s.begin(), s.end(), s.begin(), [](auto c){ return std::tolower(c); });
    return s;
}
