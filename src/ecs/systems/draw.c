#include <SDL2/SDL.h>

#include "draw.h"
#include "../components/position.h"
#include "../components/life.h"

void Draw(SDL_Renderer* renderer, Position* position, Life* life) {
    if (!renderer || !position) {
        return;
    }

    f32 size = 10;

    SDL_FRect rectangle = {
        .x = position->X - size / 2,
        .y = position->Y - size / 2,
        .w = size,
        .h = size,
    };

    u8 red = life ? (life->Health / life->MaxHealth) * 255 : 255;
    SDL_SetRenderDrawColor(renderer, 255, red, red, 255);
    SDL_RenderFillRectF(renderer, &rectangle);
}
