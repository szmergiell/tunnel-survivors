#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "position.h"
#include "sprite.h"
#include "types.h"

typedef struct Frame {
    SDL_Rect Rect;
    f64 Duration;
} Frame;

typedef struct Sprite {
    SDL_Texture* Texture;
    i32 W, FrameW;
    i32 H, FrameH;
    usize Count;
    Frame* Frames;
    u64 AnimationStart;
    f64 TotalTime;
} Sprite;

SDL_Texture* Sprite_load_texture(SDL_Renderer* renderer, const char* path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) {
        printf("Unable to create texutre from %s. IMG_Error: %s\n", path, IMG_GetError());
        return NULL;
    }

    return texture;
}

Sprite* Sprite_create(SDL_Renderer* renderer, char* path, usize size) {
    SDL_Texture* texture = Sprite_load_texture(renderer, path);
    if (!texture) {
        return NULL;
    }

    Sprite* sprite = calloc(sizeof(Sprite), 1);
    sprite->Texture = texture;
    sprite->FrameW = size;
    sprite->FrameH = size;

    SDL_QueryTexture(texture, NULL, NULL, &(sprite->W), &(sprite->H));
    sprite->Count = (sprite->W / sprite->FrameW);
    sprite->Frames = calloc(sizeof(Frame), sprite->Count);
    for (usize i = 0; i < sprite->Count; i++) {
        sprite->Frames[i].Duration = 0.1;
        SDL_Rect rect = {
            .x = i * sprite->FrameW,
            .y = 0,
            .w = sprite->FrameW,
            .h = sprite->FrameH
        };
        sprite->Frames[i].Rect = rect;
        sprite->TotalTime += sprite->Frames[i].Duration;
    }

    return sprite;
}

void Sprite_play(Sprite* sprite) {
    sprite->AnimationStart = SDL_GetPerformanceCounter();
}

SDL_Rect* Sprite_get_active_frame(Sprite* sprite) {
    u64 pc = SDL_GetPerformanceCounter();
    f64 elapsedSeconds = pc / (f64)SDL_GetPerformanceFrequency();
    f64 timeInFrame = fmod(elapsedSeconds, sprite->TotalTime);
    f64 spriteTime = 0.0;
    for (usize i = 0; i < sprite->Count; i++) {
        spriteTime += sprite->Frames[i].Duration;
        if (spriteTime >= timeInFrame) {
            return &(sprite->Frames[i].Rect);
        }
    }

    SDL_Rect* wholeSheet = calloc(sizeof(SDL_Rect), 1);
    wholeSheet->x = 0;
    wholeSheet->y = 0;
    wholeSheet->w = sprite->W;
    wholeSheet->h = sprite->H;

    return wholeSheet;
}

void Sprite_render(Sprite* sprite, SDL_Renderer* renderer, SDL_Rect rect) {
    SDL_Rect rect2 = {
        .x = rect.x,
        .y = rect.y,
        .w = sprite->FrameW,
        .h = sprite->FrameH,
    };
    SDL_RenderCopyEx(
            renderer,
            sprite->Texture,
            Sprite_get_active_frame(sprite),
            &rect,
            45.0,
            NULL,
            SDL_FLIP_NONE);
}

void Sprite_destroy(Sprite* sprite) {
    free(sprite->Frames);
    sprite->Frames = NULL;
    SDL_DestroyTexture(sprite->Texture);
    sprite->Texture = NULL;
    free(sprite);
    sprite = NULL;
}
