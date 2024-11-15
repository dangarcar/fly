#pragma once

#include "../engine/Window.hpp"
#include "../engine/Scene.h"
#include "../airport/Route.hpp"
#include "../game/Camera.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../engine/Gradient.h"

class TestScene : public Scene {
public:
    TestScene(Window& window): window(window), camera(window.getRenderer()) {}
    ~TestScene() = default;

    void handleInput(const InputEvent& event) override {
        if(auto* moveevent = std::get_if<MouseMoveEvent>(&event)) {
            auto [x, y] = moveevent->newPos;
            writeLog("%d %d\n", x, y);
        }

        camera.handleInput(event);
    }
    void update() override {}

    void start() override {
        renderer.start();
    }
    
    void render(float frameProgress) override {
        Coord c1 = {-150, 30}, c2 = {10, -20};

        renderer.render(camera, c1, c2, mtsDistance(c1, c2), SDL_WHITE);
    }

    long getTicksPerSecond() const override { return 30; }

private:
    Window& window;
    //unsigned VBO, VAO, EBO; ShaderProgram shader;

    air::RouteRenderer renderer;
    Camera camera;
    //unsigned texture1, texture2;
};