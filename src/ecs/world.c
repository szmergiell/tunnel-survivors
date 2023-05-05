#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <stdlib.h>

#include "components/position.h"
#include "components/life.h"
#include "systems/draw.h"
#include "systems/move.h"
#include "velocity.h"
#include "world.h"

typedef struct World {
    usize Capacity;
    usize Count;
    Position** Positions;
    Life** Lives;
    Velocity** Velocities;
    SDL_Renderer* renderer;
} World;

World* World_create(SDL_Renderer* renderer, usize capacity) {
    World* world = calloc(sizeof(World), 1);
    world->Capacity = capacity;
    world->Count = 0;
    world->Positions = calloc(sizeof(world->Positions), capacity);
    world->Velocities = calloc(sizeof(world->Velocities), capacity);
    world->Lives = calloc(sizeof(world->Lives), capacity);
    world->renderer = renderer;

    return world;
}

bool World_add_entity(World* world, Position* position, Velocity* velocity, Life* life) {
    if (world->Count == world->Capacity) {
        printf("World reached its' capacity: %zu", world->Capacity);
        return false;
    }
    world->Positions[world->Count] = position;
    world->Velocities[world->Count] = velocity;
    world->Lives[world->Count] = life;
    world->Count++;

    return true;
}

void World_update(World* world, u32 dt) {
    for (usize i = 0; i < world->Capacity; i++) {
        if (i == 0) {
            MovePlayer(world->Positions[i], world->Velocities[i]);
        } else {
            Move(world->Positions[i], world->Velocities[i], world->Positions[0]);
        }
        Draw(world->renderer, world->Positions[i]);
    }
}

void World_destroy(World *world) {
    free(world->Positions);
    world->Positions = NULL;

    free(world->Lives);
    world->Lives = NULL;

    free(world);
    world = NULL;
}
