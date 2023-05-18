#pragma once

#include "bullet.h"
#include "direction.h"
#include "life.h"
#include "position.h"
#include "shoot.h"
#include "velocity.h"
#include <SDL2/SDL_render.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

f64 Magnitude(f64 x, f64 y) {
    return sqrt(x * x + y * y);
}

Direction Perpendicular(Direction* direction) {
    Direction per = {
        .X = direction->Y,
        .Y = -(direction->X),
    };

    return per;
}

Bullet* Bullet_spawn(Position* position, Direction* direction, f64 width, Life* life) {
    if (!position || !direction || !life) {
        return NULL;
    }

    f64 directionMagnitude = Magnitude(direction->X, direction->Y);

    Direction* unitDirection = calloc(sizeof(Direction), 1);
    unitDirection->X = direction->X / directionMagnitude;
    unitDirection->Y = direction->Y / directionMagnitude;

    Direction perpendicularDirection = Perpendicular(unitDirection);

    f64 perpendicularMagnitue = Magnitude(perpendicularDirection.X, perpendicularDirection.Y);

    perpendicularDirection.X *= (width / perpendicularMagnitue);
    perpendicularDirection.Y *= (width / perpendicularMagnitue);

    f64 perpendicularWidth = Magnitude(perpendicularDirection.X, perpendicularDirection.Y);

    // printf("%f\n", perpendicularWidth);

    // TODO: move center of "bullet" to position

    Bullet* bullet = calloc(sizeof(Bullet), 1);
    bullet->Life = life;

    Velocity* velocity = calloc(sizeof(Velocity), 1);
    velocity->X = unitDirection->X * 100;
    velocity->Y = unitDirection->Y * 100;

    bullet->Velocity = velocity;

    f64 midpointX = perpendicularDirection.X / 2;
    f64 midpointY = perpendicularDirection.Y / 2;

    Position* startPosition = calloc(sizeof(Position), 1);
    startPosition->X = position->X - midpointX;
    startPosition->Y = position->Y - midpointY;

    Position* endPosition = calloc(sizeof(Position), 1);
    endPosition->X = position->X + midpointX;
    endPosition->Y = position->Y + midpointY;

    bullet->Start = startPosition;
    bullet->End = endPosition;

    return bullet;
}

void Bullet_draw(SDL_Renderer* renderer, Bullet* bullet, f64 dt) {
    if (!renderer || !bullet || !bullet->Start || !bullet->End) {
        return;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderDrawLineF(renderer, bullet->Start->X, bullet->Start->Y, bullet->End->X, bullet->End->Y);
}

void Bullet_travel(Bullet* bullet, Position** positions, Life** lives, f64 dt) {
    bullet->Start->X += bullet->Velocity->X * dt;
    bullet->Start->Y += bullet->Velocity->Y * dt;
    bullet->End->X += bullet->Velocity->X * dt;
    bullet->End->Y += bullet->Velocity->Y * dt;
    f64 dps = 1;
    f64 damage = dps * dt;
    bullet->Life->Health -= damage;
}

