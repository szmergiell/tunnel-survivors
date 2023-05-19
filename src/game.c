#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
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

typedef enum GameState {
    Start,
    Loop,
    GameOver,
    Exit,
    GameStateLength
} GameState;

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
    SDL_Texture* StartScreen;
    SDL_Texture* Background;
    SDL_Texture* Rama;
    SDL_Texture* Oni;
    GameState State;
    TTF_Font* Font;
    SDL_Texture* GameOverTextTexture;
    i32 Score;
    f32 EnemySpawnTime;
} Game;

void SpawnEnemy(Game* game, Position* playerPosition, Velocity* playerVelocity) {
    f64 rx = rand() / (f64)RAND_MAX;
    f64 ry = rand() / (f64)RAND_MAX;
    Position* position = calloc(sizeof(Position), 1);
    position->R = 50.0;
    f64 ri = rand() / (f64)RAND_MAX;

    f64 baseChance = 1.0/8.0;

    f64 upBonus = (playerVelocity->Y < 0) * baseChance;
    f64 downBonus = (playerVelocity->Y > 0) * baseChance;
    f64 rightBonus = (playerVelocity->X > 0) * baseChance;
    f64 leftBonus = (playerVelocity->X < 0) * baseChance;

    f64 sumBonus = upBonus + downBonus + rightBonus + leftBonus;
    if (sumBonus < 4 * baseChance) {
        // player is moving in one direction
        upBonus = upBonus > 0 ? upBonus * 4 : 0;
        downBonus = downBonus > 0 ? downBonus * 4 : 0;
        rightBonus = rightBonus > 0 ? rightBonus * 4 : 0;
        leftBonus = leftBonus > 0 ? leftBonus * 4 : 0;
    }
    if (sumBonus == 0) {
        baseChance = 1.0/4;
    }

    f64 upRange = baseChance + upBonus;
    f64 rightRange = baseChance + upRange + rightBonus;
    f64 downRange = baseChance + rightRange + downBonus;
    f64 leftRange = baseChance + downRange + leftBonus;

    // printf("L: %f\t U: %f\t D: %f\t R: %f\n",
    //         leftRange - downRange,
    //         upRange,
    //         downRange - rightRange,
    //         rightRange - upRange);

    if (ri <= upRange) {
        // above upper edge
        position->X = game->Width * rx;
        position->Y = 0;
    } else if (ri <= rightRange) {
        // beyond right edge
        position->X = game->Width;
        position->Y = game->Height * ry;
    } else if (ri <= downRange) {
        // below lower edge
        position->X = game->Width * rx;
        position->Y = game->Height;
    } else if (ri <= leftRange) {
        // beyond left edge
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

void Game_spawn_world(Game* game) {
    game->State = Start;

    game->World = World_create(game->Renderer, 500);

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
        SpawnEnemy(game, playerPosition, playerVelocity);
    }

    game->EnemySpawnTime = 3;
}

SDL_Texture* Game_create_text_texture(Game* game, char* text) {
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Blended(game->Font, text, color);
    if (!textSurface) {
        printf("Text '%s' could not be rendered. TTF_Error: %s\n", text, TTF_GetError());
        return NULL;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(game->Renderer, textSurface);
    if (!textTexture) {
        printf("Texture could not be created from text surface. TTF_Error: %s\n", TTF_GetError());
        return NULL;
    }

    free(textSurface);

    return textTexture;
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

    game->StartScreen = Game_load_texture(game->Renderer, "assets/loading_screen2.jpg");
    game->Background = Game_load_texture(game->Renderer, "assets/background.jpg");
    game->Rama = Game_load_texture(game->Renderer, "assets/rama.png");
    game->Oni = Game_load_texture(game->Renderer, "assets/oni.png");

    game->ScreenSurface = SDL_GetWindowSurface(game->Window);

    game->Font = TTF_OpenFont("assets/font.ttf", 72);
    if (!game->Font) {
        printf("Font could not be loaded. TTF_Error: %s\n", TTF_GetError());
        return game;
    }

    game->GameOverTextTexture = Game_create_text_texture(game, "Game over!");
    if (!game->GameOverTextTexture) {
        return game;
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

void Game_render_end_screen(Game* game) {
    u32 score = World_get_score(game->World);

    i32 w1, h1;
    SDL_QueryTexture(game->GameOverTextTexture, NULL, NULL, &w1, &h1);
    SDL_Rect firstLine = { game->Width / 2 - w1 / 2, game->Height / 2 - h1 / 2 - h1, w1, h1 };
    SDL_RenderCopy(game->Renderer, game->GameOverTextTexture, NULL, &firstLine);

    i32 w2, h2;
    char scoreTextBuffer[50];
    usize length = snprintf(scoreTextBuffer, 50, "Your score is %d.", score);
    SDL_Texture* scoreTextTexture = Game_create_text_texture(game, scoreTextBuffer);
    SDL_QueryTexture(scoreTextTexture, NULL, NULL, &w2, &h2);
    SDL_Rect secondLine = { game->Width / 2 - w2 / 2, game->Height / 2 + h2 / 2, w2, h2 };
    SDL_RenderCopy(game->Renderer, scoreTextTexture, NULL, &secondLine);
}

void Game_render_score(Game* game) {
    u32 score = World_get_score(game->World);

    i32 w, h;
    char scoreTextBuffer[16];
    usize length = snprintf(scoreTextBuffer, 16, "%d", score);
    SDL_Texture* scoreTextTexture = Game_create_text_texture(game, scoreTextBuffer);
    SDL_QueryTexture(scoreTextTexture, NULL, NULL, &w, &h);
    SDL_Rect corner = { 64 - w / 2, 64 - h / 2, w, h };
    SDL_RenderCopy(game->Renderer, scoreTextTexture, NULL, &corner);
}

void Game_render_start_screen(Game* game) {
    SDL_RenderCopy(game->Renderer, game->StartScreen, NULL, NULL);
}

void Game_render_background(Game* game, f64 dt) {
    static SDL_FRect textureRectangle = {
        .w = 1920.0,
        .h = 1080.0,
        .x = 1920.0,
        .y = 1080.0,
    };

    f64 lastX = textureRectangle.x;
    textureRectangle.x += game->PlayerVelocity->X * dt;
    if (textureRectangle.x <= 0) {
        textureRectangle.x = 1920;
    }
    if (textureRectangle.x >= 2 * 1920) {
        textureRectangle.x = 1920;
    }

    f64 lastY = textureRectangle.y;
    textureRectangle.y += game->PlayerVelocity->Y * dt;
    if (textureRectangle.y <= 0) {
        textureRectangle.y = 1080;
    }
    if (textureRectangle.y >= 2 * 1080) {
        textureRectangle.y = 1080;
    }

    f64 dx = textureRectangle.x - lastX;
    f64 dy = textureRectangle.y - lastY;
    f64 length = sqrt(dx * dx + dy * dy);

    // printf("player velocity: %f %f\n", game->PlayerVelocity->X, game->PlayerVelocity->Y);
    // printf("world velocity: %f %f %f\n", dx, dy, length);
    // printf("world velocity: %d %d\n", (i32)(game->PlayerVelocity->X * dt), (i32)(game->PlayerVelocity->Y * dt));

    SDL_Rect textIntRect = {
        .w = textureRectangle.w,
        .h = textureRectangle.h,
        .x = textureRectangle.x,
        .y = textureRectangle.y,
    };

    SDL_RenderCopy(game->Renderer, game->Background, &textIntRect, NULL);
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
                game->State = Exit;
                quit = true;
            }
            if (e.type == SDL_KEYDOWN &&
                e.key.keysym.sym == SDLK_ESCAPE) {
                if (game->State != Start) {
                    game->State = Start;
                } else {
                    game->State = Exit;
                    quit = true;
                }
            }
            if (e.type == SDL_KEYDOWN &&
                e.key.keysym.sym == SDLK_RETURN &&
                game->State == Start) {
                Game_spawn_world(game);
                game->State = Loop;
            }
        }

        SDL_SetRenderDrawColor(game->Renderer, 0, 0, 0, 0);
        SDL_RenderClear(game->Renderer);

        u64 currentSimulationTime = SDL_GetPerformanceCounter();
        f64 dt = (currentSimulationTime - lastSimulationTime) / (f64)SDL_GetPerformanceFrequency();

        if (game->State == Start) {
            Game_render_start_screen(game);
        }

        if (game->State == Loop) {
            if ((frameStart - lastEnemySpawnTime) / (f64)SDL_GetPerformanceFrequency() > game->EnemySpawnTime) {
                lastEnemySpawnTime = frameStart;
                SpawnEnemy(game, game->PlayerPosition, game->PlayerVelocity);
                if (game->EnemySpawnTime > 1) {
                    game->EnemySpawnTime -= 0.1;
                }
            }

            Game_render_background(game, dt);
            bool worldUpdated = World_update(game->World, dt);
            if (!worldUpdated) {
                game->State = GameOver;
            }

            Game_render_score(game);
        }

        if (game->State == GameOver) {
            Game_render_end_screen(game);
        }

        if (game->State == Exit) {
        }

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
    if (game->World) {
        World_destroy(game->World);
        game->World = NULL;
    }

    SDL_DestroyTexture(game->StartScreen);
    game->StartScreen = NULL;

    SDL_DestroyTexture(game->Background);
    game->Background = NULL;

    SDL_DestroyTexture(game->GameOverTextTexture);
    game->GameOverTextTexture = NULL;

    SDL_DestroyTexture(game->Oni);
    game->Oni = NULL;

    SDL_DestroyTexture(game->Rama);
    game->Rama = NULL;

    SDL_DestroyRenderer(game->Renderer);
    game->Renderer = NULL;

    SDL_DestroyWindow(game->Window);
    game->Window = NULL;

    IMG_Quit();

    TTF_CloseFont(game->Font);
    game->Font = NULL;

    TTF_Quit();

    SDL_Quit();

    free(game);
    game = NULL;
}
