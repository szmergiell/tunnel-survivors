#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <stdlib.h>

#include "components/position.h"
#include "components/life.h"
#include "systems/draw.h"
#include "world.h"

typedef struct World {
    usize Capacity;
    usize Count;
    Position** Positions;
    Life** Lives;
    SDL_Renderer* renderer;
} World;

World* World_create(SDL_Renderer* renderer, usize capacity) {
    World* world = calloc(sizeof(World), 1);
    world->Capacity = capacity;
    world->Count = 0;
    world->Positions = calloc(sizeof(Position), capacity);
    world->Lives = calloc(sizeof(Life), capacity);
    world->renderer = renderer;

    return world;
}

bool World_add_entity(World* world, Position* position, Life* life) {
    if (world->Count == world->Capacity) {
        printf("World reached its' capacity: %zu", world->Capacity);
        return false;
    }
    world->Positions[world->Count] = position;
    world->Lives[world->Count] = life;
    world->Count++;

    return true;
}

void World_update(World* world, u32 dt) {
    for (usize i = 0; i < world->Capacity; i++) {
        Draw(world->renderer, world->Positions[i]);
    }
}
