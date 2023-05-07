#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>

#include "direction.h"
#include "ecs/components/controller.h"
#include "ecs/components/target.h"
#include "ecs/components/position.h"
#include "ecs/components/velocity.h"
#include "ecs/components/life.h"
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
    // TODO: refactor this
    Position* PlayerPosition;
} Game;

void SpawnEnemy(Game* game, Position* playerPosition) {
        f32 rx = rand() / (f32)RAND_MAX;
        f32 ry = rand() / (f32)RAND_MAX;
        Position* position = calloc(sizeof(Position), 1);
        f32 ri = rand() / (f32)RAND_MAX;
        if (ri <= 0.25) {
            position->X = game->Width * rx;
            position->Y = 0;
        } else if (ri <= 0.50) {
            position->X = game->Width;
            position->Y = game->Height * ry;
        } else if (ri <= 0.75) {
            position->X = game->Width * rx;
            position->Y = game->Height;
        } else if (ri <= 1.00) {
            position->X = 0;
            position->Y = game->Height * ry;
        }

        Velocity* velocity = calloc(sizeof(Velocity), 1);

        Target* target = calloc(sizeof(Target), 1);
        // TODO: hard-coded player entity ID
        target->TargetId = 0;
        target->Position = playerPosition;
        target->Direction = calloc(sizeof(Direction), 1);
        Life* life = calloc(sizeof(Life), 1);
        life->Health = 180;

        World_add_entity(game->World, NULL, target, position, velocity, life);
}

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

    game->Renderer = SDL_CreateRenderer(
            game->Window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!game->Renderer) {
        printf("Renderer could not be created. SDL_Error: %s\n", SDL_GetError());
        return game;
    }

    game->ScreenSurface = SDL_GetWindowSurface(game->Window);

    game->World = World_create(game->Renderer, 100);

    Controller* playerController = calloc(sizeof(Controller), 1);
    Position* playerPosition = calloc(sizeof(Position), 1);
    playerPosition->X = game->Width / 2.0;
    playerPosition->Y = game->Height / 2.0;
    game->PlayerPosition = playerPosition;

    Velocity* playerVelocity = calloc(sizeof(Velocity), 1);
    Target* playerTarget = calloc(sizeof(Target), 1);
    Life* playerLife = calloc(sizeof(Life), 1);
    playerLife->Health = 180;

    World_add_entity(game->World, playerController, playerTarget, playerPosition, playerVelocity, playerLife);

    for (usize i = 1; i < 10; i++) {
        SpawnEnemy(game, playerPosition);
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

    f32 targetFps = 60;
    f64 targetFrameTime = 1000.0 / targetFps;

    u64 lastEnemySpawnTime = SDL_GetTicks64();

    while (!quit) {
        u64 frameStart = SDL_GetTicks64();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN &&
                e.key.keysym.sym == SDLK_ESCAPE) {
                quit = true;
            }
        }

        if (frameStart - lastEnemySpawnTime > 5000) {
            lastEnemySpawnTime = frameStart;
            SpawnEnemy(game, game->PlayerPosition);
        }

        SDL_SetRenderDrawColor(game->Renderer, 0, 0, 0, 0);
        SDL_RenderClear(game->Renderer);

        World_update(game->World);

        SDL_RenderPresent(game->Renderer);

        u64 loopTime = SDL_GetTicks64() - frameStart;
        f32 loopFps = (loopTime > 0) ? 1000.0 / loopTime : 0.0;

        f64 delay = targetFrameTime - (SDL_GetTicks64() - frameStart);
        if (delay > 0) {
            SDL_Delay(delay);
        }

        u64 delayedFrameTime = SDL_GetTicks64() - frameStart;
        f32 realFps = (delayedFrameTime > 0) ? 1000.0L / delayedFrameTime : 0.0;
        // printf("target frame time: %f, real fps: %f, loop time: %lu, loop fps: %f\n", targetFrameTime, realFps, loopTime, loopFps);
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
