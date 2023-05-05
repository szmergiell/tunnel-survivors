#pragma once

#include <stdbool.h>
#include <SDL2/SDL.h>

#include "components/position.h"
#include "components/life.h"
#include "velocity.h"

typedef struct World World;

World* World_create(SDL_Renderer* renderer, usize capacity);
bool World_add_entity(World* world, Position* position, Velocity* velocity, Life* life);
void World_update(World* world, u32 dt);
void World_destroy(World* world);
