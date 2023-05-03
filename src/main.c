#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "game.h"

int main(int argc, char* args[]) {
    if (!game_init()) {
        return -1;
    }

    if (!game_load_media()) {
        return -1;
    }

    game_loop();

    game_close();
    return 0;
}
