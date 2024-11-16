#include "engine/Window.hpp"
#include "scenes/StartMenu.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    Window window;
    if(window.start(false) != 0)
        return -1;

    auto start = std::make_unique<StartMenu>(window);
    start->start();
    window.setScene(std::move(start));

    window.run();

    return 0;
}