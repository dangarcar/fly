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

class TestScene : public Scene {
public:
    TestScene(Window& window): window(window) {}
    ~TestScene() = default;

    void handleInput(const InputEvent& event) override {
        if(auto* moveevent = std::get_if<MouseMoveEvent>(&event)) {
            auto [x, y] = moveevent->newPos;
            writeLog("%d %d\n", x, y);
        }
    }
    void update() override {}

    void start() override {
        /*float vertices[] = {
             0.5f,  0.5f,   1.0f, 1.0f,   // top right
             0.5f, -0.5f,   1.0f, 0.0f,   // bottom right
            -0.5f, -0.5f,   0.0f, 0.0f,   // bottom left
            -0.5f,  0.5f,   0.0f, 1.0f    // top left 
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
        glEnableVertexAttribArray(1);*/

        manager = TextureManager();
        manager->start();
        manager->loadTexture("HELLO", "./assets/countries/ESP.png");
    }
    
    void render(float frameProgress) override {
        //shader.use();

        //auto proj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, 1.0f, -1.0f);
        //glm::mat4 proj = glm::identity();
        //auto location = glGetUniformLocation(shaderProgram.getId(), "projection");
        //glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(proj));

        /*glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);*/

        auto rect = SDL_FRect {0.0f, 0.0f, 300.0f, 300.0f};
        manager->render(window.getRenderer(), "HELLO", 200, 200, &rect);
        manager->render(window.getRenderer(), "HELLO", 600, 200, 0.5f, 45.0f, false, SDL_GOLD);
        manager->fillRect(window.getRenderer(), {20, 20, 200, 200}, SDL_SILVER);

        window.getRenderer().renderText("HELLO world!\npatata 231", 640, 0, 50, Aligment::RIGHT, SDL_WHITE);
        window.getRenderer().renderTextRotated("HELLO rotations", 640, 0, 50, 30, SDL_GOLD);
    }

    long getTicksPerSecond() const override { return 30; }

private:
    Window& window;
    //unsigned VBO, VAO, EBO; ShaderProgram shader;

    std::optional<TextureManager> manager;
    //unsigned texture1, texture2;
};