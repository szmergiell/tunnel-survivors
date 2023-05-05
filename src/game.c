#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>

#include "ecs/components/position.h"
#include "ecs/world.h"
#include "game.h"
#include "types.h"

typedef struct Game {
    u32 Width;
    u32 Height;
    SDL_Window* Window;
    SDL_Renderer* Renderer;
    SDL_Surface* ScreenSurface;
    World* World;
} Game;

Game* Game_create(void) {
    Game* game = calloc(sizeof(Game), 1);
    game->Width = 1920;
    game->Height = 1080;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialized. SDL_Error: %s\n", SDL_GetError());
        return game;
    }

    if (TTF_Init() < 0) {
        printf("SDL TTF could not be initialized. TTF_Error: %s\n", TTF_GetError());
        return game;
    }

    i32 imageInitFlags = IMG_INIT_JPG;
    if (IMG_Init(imageInitFlags) != imageInitFlags) {
        printf("SDL Image could not be initialized. IMG_Error: %s\n", IMG_GetError());
        return game;
    }

    game->Window = SDL_CreateWindow(
            "Tunel Survivors",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            game->Width,
            game->Height,
            SDL_WINDOW_SHOWN);

    if (!game->Window) {
        printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
        return game;
    }

    game->Renderer = SDL_CreateRenderer(game->Window, -1, SDL_RENDERER_ACCELERATED);
    if (!game->Renderer) {
        printf("Renderer could not be created. SDL_Error: %s\n", SDL_GetError());
        return game;
    }

    game->ScreenSurface = SDL_GetWindowSurface(game->Window);

    game->World = World_create(game->Renderer, 100);

    for (usize i = 0; i < 100; i++) {
        f32 rx = rand() / (f32)RAND_MAX;
        f32 ry = rand() / (f32)RAND_MAX;
        Position* position = calloc(sizeof(Position), 1);
        position->X = rx * game->Width;
        position->Y = ry * game->Height;

        World_add_entity(game->World, position, NULL);
    }

    return game;
}

SDL_Surface* Game_load_image(SDL_Surface* screenSurface, const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("Unable to load image %s. IMG_Error: %s\n", path, IMG_GetError());
        return NULL;
    }

    SDL_Surface* optimizedSurface = SDL_ConvertSurface(surface, screenSurface->format, 0);
    if (!optimizedSurface) {
        printf("Unable to optimize image %s. SDL_Error: %s\n", path, SDL_GetError());
        return NULL;
    }

    SDL_FreeSurface(surface);

    return optimizedSurface;
}

SDL_Texture* Game_load_texture(SDL_Renderer* renderer, const char* path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) {
        printf("Unable to create texutre from %s. IMG_Error: %s\n", path, IMG_GetError());
        return NULL;
    }

    return texture;
}

void Game_display_full_screen_surface(Game* game, SDL_Surface* surface) {
    SDL_Rect stretchRect;
    stretchRect.w = game->Width;
    stretchRect.h = game->Height;
    SDL_BlitScaled(surface, NULL, game->ScreenSurface, &stretchRect);
    SDL_UpdateWindowSurface(game->Window);
}

void Game_display_texture(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void Game_loop(Game* game) {
    SDL_Event e;
    bool quit = false;

    while (!quit) {
        u64 frameStart = SDL_GetTicks64();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(game->Renderer, 0, 0, 0, 0);
        SDL_RenderClear(game->Renderer);

        World_update(game->World, 1);

        SDL_RenderPresent(game->Renderer);

        u64 loopTime = SDL_GetTicks64() - frameStart;
        f32 loopFps = (loopTime > 0) ? 1000.0L / loopTime : 0.0;
        f32 targetFps = 60;
        f64 targetFrameTime = 1000.0L / targetFps;
        f64 delay = targetFrameTime - (SDL_GetTicks64() - frameStart);
        if (delay > 0) {
            SDL_Delay(delay);
        }

        u64 delayedFrameTime = SDL_GetTicks64() - frameStart;
        f32 realFps = (delayedFrameTime > 0) ? 1000.0L / delayedFrameTime : 0.0;
        printf("target frame time: %f, real fps: %f, frame time: %lu, loop fps: %f\n", targetFrameTime, realFps, loopTime, loopFps);
    }
}

void Game_destroy(Game* game) {
    World_destroy(game->World);
    game->World = NULL;

    SDL_DestroyRenderer(game->Renderer);
    game->Renderer = NULL;

    SDL_DestroyWindow(game->Window);
    game->Window = NULL;

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    free(game);
    game = NULL;
}
