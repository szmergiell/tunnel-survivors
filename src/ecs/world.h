#pragma once

#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "components/position.h"
#include "components/life.h"
#include "controller.h"
#include "direction.h"
#include "target.h"
#include "velocity.h"

typedef struct World World;

World* World_create(SDL_Renderer* renderer, usize capacity);

bool World_add_entity(
    World* world,
    Controller* controller,
    Target* target,
    Position* position,
    Velocity* velocity,
    Life* life,
    SDL_Texture* texture);

void World_update(World* world, f64 dt);

void World_destroy(World* world);
