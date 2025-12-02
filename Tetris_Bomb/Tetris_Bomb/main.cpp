#include "Game.h"

int main() {
    system("mode con: cols=120 lines=40");
    Tetris::Game game;
    game.run();
    return 0;
}