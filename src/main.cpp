#include "engine/Window.hpp"
#include "game/Game.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    Window window;
    if(window.start(false) != 0)
        return -1;

    auto game = std::make_unique<Game>(window.getWidth(), window.getHeight());
    window.setScene(std::move(game));

    window.run();

    return 0;
}