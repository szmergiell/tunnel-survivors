#pragma once

#include <SDL2/SDL.h>

#include "../components/position.h"
#include "../components/life.h"

void Draw(SDL_Renderer* renderer, Position* position, Life* life);
