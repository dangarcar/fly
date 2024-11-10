#include "engine/Window.hpp"
#include "scenes/StartMenu.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    Window window;
    if(window.start(false) != 0)
        return -1;

    auto testScene = std::make_unique<StartMenu>(window);
    testScene->start();
    window.setScene(std::move(testScene));

    window.run();

    return 0;
}