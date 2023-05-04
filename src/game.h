#pragma once
#include <SDL2/SDL.h>

typedef struct Game Game;

Game* Game_create(void);
void Game_loop(struct Game* game);
void Game_destroy(struct Game* game);
