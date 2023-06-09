#pragma once

#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "components/position.h"
#include "components/life.h"
#include "components/controller.h"
#include "components/direction.h"
#include "components/target.h"
#include "components/velocity.h"

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

void World_set_player_target(World* world, Position* position);

void World_spawn_bullet(World* world);

bool World_update(World* world, f64 dt);

void World_destroy(World* world);

u32 World_get_score(World* world);
