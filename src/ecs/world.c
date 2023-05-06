#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "components/position.h"
#include "components/target.h"
#include "components/life.h"
#include "components/controller.h"
#include "components/velocity.h"
#include "systems/control.h"
#include "systems/aim.h"
#include "systems/draw.h"
#include "systems/move.h"
#include "types.h"
#include "world.h"

typedef struct World {
    usize Capacity;
    usize Count;
    Controller** Controllers;
    Target** Targets;
    Position** Positions;
    Life** Lives;
    Velocity** Velocities;
    SDL_Renderer* renderer;
} World;

World* World_create(SDL_Renderer* renderer, usize capacity) {
    World* world = calloc(sizeof(World), 1);
    world->Capacity = capacity;
    world->Count = 0;
    world->Controllers = calloc(sizeof(Controller*), capacity);
    world->Targets = calloc(sizeof(Target*), capacity);
    world->Positions = calloc(sizeof(Position*), capacity);
    world->Velocities = calloc(sizeof(Velocity*), capacity);
    world->Lives = calloc(sizeof(Life*), capacity);
    world->renderer = renderer;

    return world;
}

bool World_add_entity(
        World* world,
        Controller* controller,
        Target* target,
        Position* position,
        Velocity* velocity,
        Life* life) {
    if (world->Count == world->Capacity) {
        printf("World reached its' capacity: %zu", world->Capacity);
        return false;
    }

    // TODO: we have a continuous memory range for pointers
    // but not necessarily a continuous memory range for actual values,
    // since they are allocated outside of this "class".
    world->Controllers[world->Count] = controller;
    world->Targets[world->Count] = target;
    world->Positions[world->Count] = position;
    world->Velocities[world->Count] = velocity;
    world->Lives[world->Count] = life;
    world->Count++;

    return true;
}

void World_update(World* world) {
    for (usize i = 0; i < world->Capacity; i++) {
        Control(world->Controllers[i], world->Velocities[i]);
        Aim(world->Positions[i], world->Targets[i], world->Velocities[i]);
        Move(world->Positions[i], world->Velocities[i]);
        Draw(world->renderer, world->Positions[i]);
    }
}

void World_destroy(World *world) {
    for (usize i = 0; i < world->Capacity; i++) {
        free(world->Controllers[i]);
        world->Controllers[i] = NULL;

        free(world->Targets[i]);
        world->Targets[i] = NULL;

        free(world->Positions[i]);
        world->Positions[i] = NULL;

        free(world->Velocities[i]);
        world->Velocities[i] = NULL;

        free(world->Lives[i]);
        world->Lives[i] = NULL;
    }

    free(world->Controllers);
    world->Controllers = NULL;

    free(world->Targets);
    world->Targets = NULL;

    free(world->Positions);
    world->Positions = NULL;

    free(world->Velocities);
    world->Velocities = NULL;

    free(world->Lives);
    world->Lives = NULL;

    free(world);
    world = NULL;
}
