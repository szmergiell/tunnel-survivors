#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "game.h"

int main(int argc, char* args[]) {
    Game* game = Game_create();

    Game_loop(game);

    Game_destroy(game);

    return 0;
}
