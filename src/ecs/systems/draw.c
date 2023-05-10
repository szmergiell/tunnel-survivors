#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include "draw.h"
#include "../components/position.h"
#include "../components/life.h"

void Draw(SDL_Renderer* renderer, Position* position, Life* life, SDL_Texture* texture) {
    if (!renderer || !position) {
        return;
    }

    SDL_Rect rectangle = {
        .x = position->X - (100 / 2),
        .y = position->Y - (200 / 2),
        .w = 100,
        .h = 200,
    };

    if (texture) {
        SDL_RenderCopy(renderer, texture, NULL, &rectangle);
    } else {
        u8 red = life ? (life->Health / life->MaxHealth) * 255 : 255;
        SDL_SetRenderDrawColor(renderer, 255, red, red, 255);
        SDL_RenderFillRect(renderer, &rectangle);
    }
}
