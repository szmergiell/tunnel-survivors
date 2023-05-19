#pragma once

#include "bullet.h"
#include "direction.h"
#include "life.h"
#include "position.h"
#include <SDL2/SDL_render.h>

Bullet* Bullet_spawn(Position* position, Direction* direction, f64 width, Life* life, usize worldCapacity);
void Bullet_draw(SDL_Renderer* renderer, Bullet* bullet, f64 dt);
void Bullet_travel(Bullet* bullet, Position** positions, Life** lives, f64 dt);
bool Bullet_collide(Bullet* bullet, usize entityId, Position* position, Life* life);
