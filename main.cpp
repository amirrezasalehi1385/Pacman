#include "include/Game.h"
#include <SDL2/SDL_image.h>

int SDL_main(int argc, char* argv[]) {
    Game game;
    if(game.init("Pacman", 448, 576)) {
        game.run();
    }
    return 0;
}
