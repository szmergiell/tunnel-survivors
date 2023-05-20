#pragma once

#include "types.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>

typedef struct Frame Frame;

typedef struct Sprite Sprite;

Sprite* Sprite_create(SDL_Renderer* renderer, char* path, usize size);

void Sprite_play(Sprite* sprite);

void Sprite_render(Sprite* sprite, SDL_Renderer* renderer, SDL_Rect rect, f64 angle);

void Sprite_destroy(Sprite* sprite);
