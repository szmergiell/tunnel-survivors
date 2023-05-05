#include <SDL2/SDL.h>

#include "../components/position.h"

void Draw(SDL_Renderer* renderer, Position* position) {
    if (!renderer || !position) {
        return;
    }

    SDL_FRect rectangle = {
        .x = position->X,
        .y = position->Y,
        .w = 10,
        .h = 10
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRectF(renderer, &rectangle);
}
