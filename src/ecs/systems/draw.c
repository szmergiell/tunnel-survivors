#include <SDL2/SDL.h>

#include "../components/position.h"

void Draw(SDL_Renderer* renderer, Position* position) {
    if (!position) {
        return;
    }

    SDL_Rect rectangle = {
        .x = position->X,
        .y = position->Y,
        .w = 100,
        .h = 100
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rectangle);
}
