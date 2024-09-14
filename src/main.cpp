#include "engine/Window.hpp"
#include "Game.hpp"

int main(int argc, char *argv[]) {
    Window window;
    if(window.start() != 0)
        return -1;

    Camera camera(window.getWidth(), window.getHeight());
    auto game = std::make_unique<Game>(Game(camera));
    window.setScene(std::move(game));

    window.run();

    return 0;
}