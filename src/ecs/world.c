#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "components/bullet.h"
#include "systems/collide.h"
#include "components/position.h"
#include "components/target.h"
#include "components/life.h"
#include "components/controller.h"
#include "components/velocity.h"
#include "components/direction.h"
#include "../game.h"
#include "systems/moveWorld.h"
#include "systems/shoot.h"
#include "systems/chooseTarget.h"
#include "systems/aim.h"
#include "systems/faceTarget.h"
#include "systems/control.h"
#include "systems/attack.h"
#include "systems/draw.h"
#include "systems/move.h"
#include "../types.h"
#include "world.h"

typedef struct World {
    usize Capacity;
    usize Count;
    Controller** Controllers;
    Target** Targets;
    Position** Positions;
    Life** Lives;
    Velocity** Velocities;
    Bullet* Bullet;
    SDL_Texture** Textures;
    SDL_Renderer* renderer;
    u32 Score;
    u64 BulletSpawnTicks;
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

typedef struct Entity {
    usize Id;
    Position* Position;
    Life* Life;
    SDL_Texture* Texture;
} Entity;

i32 CompareEntityPositions(const void* e1, const void* e2) {
    Entity* p1 = (Entity*)e1;
    Entity* p2 = (Entity*)e2;

    if (p1->Position && !p2->Position) {
        return -1;
    } else if (!p1->Position && p2->Position) {
        return 1;
    } else if (!p1->Position && !p2->Position) {
        return 0;
    }

    if (p1->Position->Y < p2->Position->Y) {
        return -1;
    } else if (p1->Position->Y > p2->Position->Y) {
        return 1;
    } else {
        return 0;
    }
}

Entity* SortEntitiesByPosition(World* world) {
    Entity* entities = calloc(sizeof(Entity), world->Capacity);

    for (usize i = 0; i < world->Capacity; i++) {
        entities[i].Id = i;
        entities[i].Position = world->Positions[i];
        entities[i].Life = world->Lives[i];
        entities[i].Texture = world->Textures[i];
    }

    qsort(entities, world->Capacity, sizeof(Entity), &CompareEntityPositions);

    return entities;
}

void World_set_player_target(World* world, Position* position) {
    world->Targets[0]->Position = position;
    f64 distanceX = position->X - world->Positions[0]->X;
    f64 distanceY = position->Y - world->Positions[0]->Y;
    f64 distance = sqrt(distanceX * distanceX + distanceY * distanceY);
    world->Targets[0]->Direction->X = distanceX / distance;
    world->Targets[0]->Direction->Y = distanceY / distance;
    world->Targets[0]->Distance = distance;
    world->Targets[0]->Angle = atan2(world->Targets[0]->Direction->Y, world->Targets[0]->Direction->X);
    world->Targets[0]->Angle *= 180.0 / M_PI;
}

void World_spawn_bullet(World* world) {
    u64 perfCounter = SDL_GetPerformanceCounter();
    f64 secondsSinceLastBulletSpawn = (perfCounter - world->BulletSpawnTicks);
    secondsSinceLastBulletSpawn /= (f64)SDL_GetPerformanceFrequency();

    if (secondsSinceLastBulletSpawn < 3) {
        return;
    }

    Life* life = calloc(sizeof(Life), 1);
    life->MaxHealth = 0.6;
    life->Health = 0.6;
    Bullet* bullet = Bullet_spawn(
            world->Positions[0],
            world->Targets[0]->Direction,
            164,
            life,
            world->Capacity,
            world->renderer);
    // printf("bullet spawned");
    world->Bullet = bullet;
    world->BulletSpawnTicks = perfCounter;
}

bool World_update(World* world, f64 dt) {
    for (usize i = 0; i < world->Capacity; i++) {
        // TODO: controller component is a being used as a proxy / tag
        // for identitfying player entity..
        Control(world->Controllers[i], world->Velocities[i], world);

        if (i == 0) {
            CollidePlayer(i, world->Positions, world->Velocities, world->Capacity, dt);
        }

        if (i != 0) {
            MoveWorld(world->Positions[i], world->Velocities[0], dt);
        }

        if (i == 0) {
            // ChooseTarget(i, world->Controllers[i], world->Targets[i], world->Positions, world->Capacity);
        }

        Aim(world->Positions[i], world->Targets[i]);

        if (i != 0) {
            FaceTarget(world->Targets[i], world->Velocities[i]);
        }

        // Attack(world->Targets[i], world->Lives, dt);

        if (i != 0) {
            CollideWorld(i, world->Positions, world->Velocities, world->Targets, world->Lives, world->Capacity, dt);
            Move(world->Positions[i], world->Velocities[i], dt);
        }
    }

    Entity* entities = SortEntitiesByPosition(world);

    for (usize i = 0; i < world->Capacity; i++) {
        Entity entity = entities[i];
        usize id = entity.Id;
        // printf("%zu %f\n", id, world->Positions[id] ? world->Positions[id]->Y : -333);
        Draw(world->renderer, world->Positions[id], world->Lives[id], world->Textures[id]);
    }

    if (world->Bullet) {
        MoveWorld(world->Bullet->Start, world->Velocities[0], dt);
        MoveWorld(world->Bullet->End, world->Velocities[0], dt);
        Bullet_travel(world->Bullet, world->Positions, world->Lives, dt);
        Bullet_draw(world->renderer, world->Bullet, dt);
    }

    if (world->Bullet && world->Bullet->Life->Health <= 0) {
        free(world->Bullet->Life);
        world->Bullet->Life = NULL;
        free(world->Bullet->Start);
        world->Bullet->Start = NULL;
        free(world->Bullet->End);
        world->Bullet->End = NULL;
        free(world->Bullet->EntitiesHit);
        world->Bullet->EntitiesHit = NULL;
        free(world->Bullet->Velocity);
        world->Bullet->Velocity = NULL;
        free(world->Bullet);
        world->Bullet = NULL;
    }

    for (usize i = 0; i < world->Capacity; i++) {
        if (i != 0 && world->Bullet) {
            Bullet_collide(world->Bullet, i, world->Positions[i], world->Lives[i]);
        }
        if (world->Lives[i] && world->Lives[i]->Health <= 0) {
            world->Count--;

            free(world->Controllers[i]);
            world->Controllers[i] = world->Controllers[world->Count];

            free(world->Targets[i]);
            world->Targets[i] = world->Targets[world->Count];

            free(world->Positions[i]);
            world->Positions[i] = world->Positions[world->Count];

            free(world->Velocities[i]);
            world->Velocities[i] = world->Velocities[world->Count];

            free(world->Lives[i]);
            world->Lives[i] = world->Lives[world->Count];

            // free(world->Textures[i]);
            world->Textures[i] = world->Textures[world->Count];

            world->Controllers[world->Count] = NULL;

            world->Targets[world->Count] = NULL;

            world->Positions[world->Count] = NULL;

            world->Velocities[world->Count] = NULL;

            world->Lives[world->Count] = NULL;

            world->Textures[world->Count] = NULL;

            if (i != 0) {
                world->Score++;
            } else {
                return false;
            }
        }
    }

    return true;
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

    free(world->Bullet);
    world->Bullet = NULL;

    free(world);
    world = NULL;
}

u32 World_get_score(World* world) {
    return world->Score;
}
