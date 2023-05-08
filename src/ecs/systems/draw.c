#include <SDL2/SDL.h>

#include "draw.h"
#include "../components/position.h"
#include "../components/life.h"

void Draw(SDL_Renderer* renderer, Position* position, Life* life) {
    if (!renderer || !position) {
        return;
    }

    SDL_FRect rectangle = {
        .x = position->X,
        .y = position->Y,
        .w = 10,
        .h = 10
    };

    u8 red = life ? (life->Health / life->MaxHealth) * 255 : 255;
    SDL_SetRenderDrawColor(renderer, 255, red, red, 255);
    SDL_RenderFillRectF(renderer, &rectangle);
}
