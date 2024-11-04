#pragma once

#include "../engine/Window.hpp"
#include "../engine/Scene.h"
#include "../engine/ShaderProgram.hpp"
#include "../engine/TextRenderer.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../engine/Gradient.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

class TestScene : public Scene {
public:
    TestScene(Window& window): window(window), shaderProgram(std::filesystem::path{"./src/shaders/test.vs"}, std::filesystem::path{"./src/shaders/fragment.fs"}) {}
    ~TestScene() = default;

    void handleInput(const InputEvent& event) override {
        if(auto* moveevent = std::get_if<MouseMoveEvent>(&event)) {
            auto [x, y] = moveevent->newPos;
            //writeLog("%d %d\n", x, y);
        }
    }
    void start() override {
        /*float vertices[] = {
            -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
             0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
             0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,   0.0f, 1.0f
        };
        float vertices[] = {
            200.0f, 200.0f,
            500.0f, 200.0f,
            500.0f, 500.0f,
            200.0f, 500.0f
        };
        int indices[] = {
            0, 1, 2, 2, 3, 0
        };*/


        /*auto vertices = text.getChar('a').vertices; //TODO: make it work
        auto indices = text.getChar('a').indices;

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(int), &indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        /*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);*/

        /*glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);  
        uint8_t* data = stbi_load("./assets/countries/PRT.png", &width, &height, &nrChannels, 0);
        if(data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            writeError("Image couldn't be loaded\n");
        }
        stbi_image_free(data);

        glActiveTexture(GL_TEXTURE1);
        glGenTextures(1, &texture2);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        data = stbi_load("./assets/countries/ESP.png", &width, &height, &nrChannels, 0);
        if(data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            writeError("Image couldn't be loaded\n");
        }
        stbi_image_free(data);*/
    }
    void update() override {}
    void render(float frameProgress) override {
        /*shaderProgram.use();

        //auto proj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, 1.0f, -1.0f);
        //glm::mat4 proj = glm::identity();
        //auto location = glGetUniformLocation(shaderProgram.getId(), "projection");
        //glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, text.getChar('a').indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);*/

        window.getRenderer().renderText("HELLO world!\npatata 231", 640, 0, 50, Aligment::RIGHT, SDL_WHITE);
        window.getRenderer().renderTextRotated("HELLO rotations", 640, 0, 50, 30, SDL_GOLD);
    }

    long getTicksPerSecond() const override { return 30; }

private:
    Window& window;
    ShaderProgram shaderProgram;
    unsigned VBO, VAO, EBO;
    //unsigned texture1, texture2;
};