#include "engine/Window.hpp"
#include "Game.hpp"

int main(int argc, char *argv[]) {
    Window window;
    if(window.start() != 0)
        return -1;

    auto game = Game::createGame(window);
    window.setScene(std::move(game));

    window.run();

    return 0;
}