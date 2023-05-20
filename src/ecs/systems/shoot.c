#pragma once

#include "bullet.h"
#include "direction.h"
#include "life.h"
#include "position.h"
#include "shoot.h"
#include "sprite.h"
#include "velocity.h"
#include <SDL2/SDL_render.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wchar.h>

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

Bullet* Bullet_spawn(
        Position* position,
        Direction* direction,
        f64 width,
        Life* life,
        usize worldCapacity,
        SDL_Renderer* renderer) {
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
    bullet->Width = width;
    bullet->Life = life;
    bullet->LifeDecayPerSecond = 1;
    bullet->Damage = 3;
    bullet->DamageDecresePerTarget = 1;
    bullet->EntitiesHit = calloc(sizeof(bool), worldCapacity);

    bullet->Speed = 500;

    Velocity* velocity = calloc(sizeof(Velocity), 1);
    velocity->X = unitDirection->X * bullet->Speed;
    velocity->Y = unitDirection->Y * bullet->Speed;
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

    bullet->Sprite = Sprite_create(renderer, "assets/dragon-slash.png", 32);
    Sprite_play(bullet->Sprite);

    return bullet;
}

SDL_Rect Bullet_get_rectangle(Position* p1, Position* p2) {
    f64 left = fmin(p1->X, p2->X);
    f64 right = fmax(p1->X, p2->X);
    f64 top = fmin(p1->Y, p2->Y);
    f64 bottom = fmax(p1->Y, p2->Y);
    f64 width = right - left;
    f64 height = bottom - top;
    SDL_Rect rect = {
        .x = left,
        .y = top,
        .w = width,
        .h = height,
    };
    return rect;
}

void Bullet_draw(SDL_Renderer* renderer, Bullet* bullet, f64 dt) {
    if (!renderer || !bullet || !bullet->Start || !bullet->End) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderDrawLineF(renderer, bullet->Start->X, bullet->Start->Y, bullet->End->X, bullet->End->Y);

    SDL_Rect rect = {
        .x = ((bullet->End->X + bullet->Start->X) / 2.0) - bullet->Width / 2,
        .y = ((bullet->End->Y + bullet->Start->Y) / 2.0) - bullet->Width / 2,
        .w = bullet->Width,
        .h = bullet->Width,
    };
    Sprite_render(
            bullet->Sprite,
            renderer,
            rect);
}

void Bullet_travel(Bullet* bullet, Position** positions, Life** lives, f64 dt) {
    bullet->Start->X += bullet->Velocity->X * dt;
    bullet->Start->Y += bullet->Velocity->Y * dt;
    bullet->End->X += bullet->Velocity->X * dt;
    bullet->End->Y += bullet->Velocity->Y * dt;
    bullet->Life->Health -= bullet->LifeDecayPerSecond * dt;
}

bool Bullet_collide(Bullet* bullet, usize entityId, Position* position, Life* life) {
    if (!position || !life) {
        return false;
    }
    if (bullet->Damage <= 0) {
        // printf("Bullet damage is zero\n");
        return false;
    }
    if (bullet->EntitiesHit[entityId]) {
        return false;
    }

    f64 U = (position->X - bullet->Start->X) * (bullet->End->X - bullet->Start->X) +
            (position->Y - bullet->Start->Y) * (bullet->End->Y - bullet->Start->Y);

    U /= (bullet->Width * bullet->Width);

    if (U < 0.0 || U > 1.0) {
        return false;
    }

    f64 intersectionX = bullet->Start->X + U * (bullet->End->X - bullet->Start->X);
    f64 intersectionY = bullet->Start->Y + U * (bullet->End->Y - bullet->Start->Y);

    f64 distance = Magnitude(position->X - intersectionX, position->Y - intersectionY);

    if (distance > position->R) {
        return false;
    }

    life->Health -= bullet->Damage;
    // printf("Attacking %zu with %f damage, %f health left\n", entityId, bullet->Damage, life->Health);
    bullet->Damage -= bullet->DamageDecresePerTarget;
    if (bullet->Damage < 1) {
        bullet->Damage = 1;
    }
    bullet->EntitiesHit[entityId] = true;
    return true;
}

