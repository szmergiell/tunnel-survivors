#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
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
    Velocity* PlayerVelocity;
    SDL_Texture* Background;
    SDL_Texture* Rama;
    SDL_Texture* Oni;
} Game;

void SpawnEnemy(Game* game, Position* playerPosition) {
        f64 rx = rand() / (f64)RAND_MAX;
        f64 ry = rand() / (f64)RAND_MAX;
        Position* position = calloc(sizeof(Position), 1);
        position->R = 50.0;
        f64 ri = rand() / (f64)RAND_MAX;
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
        life->MaxHealth = 3.0;
        life->Health = 3.0;

        World_add_entity(game->World, NULL, target, position, velocity, life, game->Oni);
}

SDL_Texture* Game_load_texture(SDL_Renderer* renderer, const char* path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) {
        printf("Unable to create texutre from %s. IMG_Error: %s\n", path, IMG_GetError());
        return NULL;
    }

    return texture;
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

    i32 imageInitFlags = IMG_INIT_JPG | IMG_INIT_PNG;
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

    game->Background = Game_load_texture(game->Renderer, "assets/background.jpg");
    game->Rama = Game_load_texture(game->Renderer, "assets/rama.png");
    game->Oni = Game_load_texture(game->Renderer, "assets/oni.png");

    game->ScreenSurface = SDL_GetWindowSurface(game->Window);

    game->World = World_create(game->Renderer, 100);

    Controller* playerController = calloc(sizeof(Controller), 1);
    Position* playerPosition = calloc(sizeof(Position), 1);
    playerPosition->X = game->Width / 2.0;
    playerPosition->Y = game->Height / 2.0;
    playerPosition->R = 50.0;
    game->PlayerPosition = playerPosition;

    Velocity* playerVelocity = calloc(sizeof(Velocity), 1);
    game->PlayerVelocity = playerVelocity;

    Target* playerTarget = calloc(sizeof(Target), 1);
    playerTarget->Direction = calloc(sizeof(Direction), 1);

    Life* playerLife = calloc(sizeof(Life), 1);
    playerLife->MaxHealth = 3.0;
    playerLife->Health = 3.0;

    World_add_entity(
        game->World,
        playerController,
        playerTarget,
        playerPosition,
        playerVelocity,
        playerLife,
        game->Rama);

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

void Game_render_background(Game* game, f64 dt) {
    static SDL_Rect textureRectangle = {
        .w = 1920,
        .h = 1080,
        .x = 1920,
        .y = 1080,
    };
    textureRectangle.x += (i32)(game->PlayerVelocity->X * dt);
    if (textureRectangle.x <= 0) {
        textureRectangle.x = 1920;
    }
    if (textureRectangle.x >= 2 * 1920) {
        textureRectangle.x = 1920;
    }
    textureRectangle.y += (i32)(game->PlayerVelocity->Y * dt);
    if (textureRectangle.y <= 0) {
        textureRectangle.y = 1080;
    }
    if (textureRectangle.y >= 2 * 1080) {
        textureRectangle.y = 1080;
    }

    // printf("world velocity: %d %d\n", (i32)(game->PlayerVelocity->X * dt), (int)(game->PlayerVelocity->Y * dt));

    SDL_RenderCopy(game->Renderer, game->Background, &textureRectangle, NULL);
}

void Game_loop(Game* game) {
    SDL_Event e;
    bool quit = false;

    f64 targetFps = 60;
    f64 targetFrameTime = 1.0 / targetFps;

    u64 lastEnemySpawnTime = SDL_GetPerformanceCounter();
    u64 lastSimulationTime = SDL_GetPerformanceCounter();

    while (!quit) {
        u64 frameStart = SDL_GetPerformanceCounter();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN &&
                e.key.keysym.sym == SDLK_ESCAPE) {
                quit = true;
            }
        }

        if ((frameStart - lastEnemySpawnTime) / (f64)SDL_GetPerformanceFrequency() > 3) {
            lastEnemySpawnTime = frameStart;
            SpawnEnemy(game, game->PlayerPosition);
        }

        SDL_SetRenderDrawColor(game->Renderer, 0, 0, 0, 0);
        SDL_RenderClear(game->Renderer);

        u64 currentSimulationTime = SDL_GetPerformanceCounter();
        f64 dt = (currentSimulationTime - lastSimulationTime) / (f64)SDL_GetPerformanceFrequency();

        Game_render_background(game, dt);
        World_update(game->World, dt);

        lastSimulationTime = currentSimulationTime;

        SDL_RenderPresent(game->Renderer);

        f64 loopTime = (SDL_GetPerformanceCounter() - frameStart) / (f64)SDL_GetPerformanceFrequency();
        f64 loopFps = (loopTime > 0) ? 1.0 / loopTime : 0.0;

        f64 delay = targetFrameTime - loopTime;
        if (delay > 0) {
            SDL_Delay(floor(delay * 1000));
        }

        f64 delayedFrameTime = (SDL_GetPerformanceCounter() - frameStart) / (f64)SDL_GetPerformanceFrequency();
        f64 realFps = (delayedFrameTime > 0) ? 1.0 / delayedFrameTime : 0.0;
        // printf("target frame time: %f, real fps: %f, loop time: %f, loop fps: %f\n", targetFrameTime, realFps, loopTime, loopFps);
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
