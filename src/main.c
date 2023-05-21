#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "game.h"

#define main WinMain
int main(int argc, char* args[]) {
    Game* Game = Game_create();

    Game_loop(Game);

    Game_destroy(Game);

    return 0;
}
