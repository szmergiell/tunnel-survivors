#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include "../components/position.h"
#include "../components/life.h"

void Draw(SDL_Renderer* renderer, Position* position, Life* life, SDL_Texture* texture);
