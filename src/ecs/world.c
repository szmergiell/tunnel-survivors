#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <stdlib.h>

#include "collide.h"
#include "components/position.h"
#include "components/target.h"
#include "components/life.h"
#include "components/controller.h"
#include "components/velocity.h"
#include "direction.h"
#include "game.h"
#include "moveWorld.h"
#include "systems/chooseTarget.h"
#include "systems/aim.h"
#include "systems/faceTarget.h"
#include "systems/control.h"
#include "systems/attack.h"
#include "systems/draw.h"
#include "systems/move.h"
#include "types.h"
#include "world.h"

typedef enum WorldState {
    PlayerAlive,
    PlayerDead,
    WorldStateLength
} WorldState;

typedef struct World {
    usize Capacity;
    usize Count;
    Controller** Controllers;
    Target** Targets;
    Position** Positions;
    Life** Lives;
    Velocity** Velocities;
    SDL_Texture** Textures;
    SDL_Renderer* renderer;
    i32 Score;
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
    world->Textures = calloc(sizeof(SDL_Texture*), capacity);
    world->renderer = renderer;

    return world;
}

bool World_add_entity(
        World* world,
        Controller* controller,
        Target* target,
        Position* position,
        Velocity* velocity,
        Life* life,
        SDL_Texture* texture) {
    if (world->Count == world->Capacity) {
        // printf("World reached its' capacity: %zu\n", world->Capacity);
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
    world->Textures[world->Count] = texture;
    world->Count++;

    return true;
}

u32 World_update(World* world, f64 dt) {
    for (usize i = 0; i < world->Capacity; i++) {
        // TODO: controller component is a being used as a proxy / tag
        // for identitfying player entity..
        if (i == 0) {
            CollidePlayer(i, world->Positions, world->Velocities, world->Capacity, dt);
        }
        if (i != 0) {
            MoveWorld(world->Positions[i], world->Velocities[0], dt);
        }
        ChooseTarget(i, world->Controllers[i], world->Targets[i], world->Positions, world->Capacity);
        Aim(world->Positions[i], world->Targets[i]);
        FaceTarget(world->Targets[i], world->Velocities[i]);
        Control(world->Controllers[i], world->Velocities[i]);
        Attack(world->Targets[i], world->Lives, dt);
        CollideWorld(i, world->Positions, world->Velocities, world->Targets, world->Lives, world->Capacity, dt);
        if (i != 0) {
            Move(world->Positions[i], world->Velocities[i], dt);
        }
        if (world->Lives[i] && world->Lives[i]->Health < 0) {
            world->Controllers[i] = NULL;
            world->Targets[i] = NULL;
            world->Positions[i] = NULL;
            world->Velocities[i] = NULL;
            world->Lives[i] = NULL;
            if (i != 0) {
                world->Score++;
            } else {
                return world->Score;
            }
            continue;
        }
        Draw(world->renderer, world->Positions[i], world->Lives[i], world->Textures[i]);
    }

    // TODO: pass score separately to avoid issue
    // where game does not end
    // because user scored 0
    return -1;
}

void World_destroy(World *world) {
    for (usize i = 0; i < world->Capacity; i++) {
        free(world->Controllers[i]);
        world->Controllers[i] = NULL;

        // free(world->Targets[i]->Position);
        // world->Targets[i]->Position = NULL;

        // free(world->Targets[i]->Direction);
        // world->Targets[i]->Direction = NULL;

        free(world->Targets[i]);
        world->Targets[i] = NULL;

        free(world->Positions[i]);
        world->Positions[i] = NULL;

        free(world->Velocities[i]);
        world->Velocities[i] = NULL;

        free(world->Lives[i]);
        world->Lives[i] = NULL;

        // free(world->Textures[i]);
        // world->Textures[i] = NULL;
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

    free(world->Textures);
    world->Textures = NULL;

    free(world);
    world = NULL;
}
