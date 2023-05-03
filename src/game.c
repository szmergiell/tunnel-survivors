#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

static const int SCREEN_WIDTH = 1920;
static const int SCREEN_HEIGHT = 1080;

static const char* LOADING_SCREEN_IMAGE_PATH = "assets/loading_screen.jpg";

static SDL_Window* gWindow = NULL;
static SDL_Surface* gScreenSurface = NULL;
static SDL_Surface* gSplashSurface = NULL;

bool game_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialized. SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    int imageInitFlags = IMG_INIT_JPG;
    if (IMG_Init(imageInitFlags) != imageInitFlags) {
        printf("SDL Image could not be initialized. IMG_Error: %s\n", IMG_GetError());
        return false;
    }

    gWindow = SDL_CreateWindow(
            "Tunel Survivors",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN);

    if (gWindow == NULL) {
        printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    gScreenSurface = SDL_GetWindowSurface(gWindow);
    return true;
}

SDL_Surface* game_load_image(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL) {
        printf("Unable to load image %s. IMG_Error: %s\n", path, IMG_GetError());
        return NULL;
    }

    SDL_Surface* optimizedSurface = SDL_ConvertSurface(surface, gScreenSurface->format, 0);
    if (optimizedSurface == NULL) {
        printf("Unable to optimize image %s. SDL_Error: %s\n", path, SDL_GetError());
        return NULL;
    }

    SDL_FreeSurface(surface);

    return optimizedSurface;
}

bool game_load_media(void) {
    gSplashSurface = game_load_image(LOADING_SCREEN_IMAGE_PATH);
    if (gScreenSurface == NULL) {
        return false;
    }

    return true;
}

void game_display_loading_screen(void) {
    SDL_Rect stretchRect;
    stretchRect.w = SCREEN_WIDTH;
    stretchRect.h = SCREEN_HEIGHT;
    SDL_BlitScaled(gSplashSurface, NULL, gScreenSurface, &stretchRect);
    SDL_UpdateWindowSurface(gWindow);
}

void game_loop(void) {
    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        game_display_loading_screen();
    }
}

void game_close(void) {
    SDL_FreeSurface(gSplashSurface);
    gSplashSurface = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    IMG_Quit();
    SDL_Quit();
}
