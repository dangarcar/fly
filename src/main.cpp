#include "engine/Window.hpp"
#include "game/Game.hpp"

int main(int argc, char *argv[]) {
    Window window;
    if(window.start() != 0)
        return -1;

    auto game = std::make_unique<Game>(window.getWidth(), window.getHeight());
    window.setScene(std::move(game));

    window.run();

    return 0;
}