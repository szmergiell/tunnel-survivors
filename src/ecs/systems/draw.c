#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include "draw.h"
#include "../components/position.h"
#include "../components/life.h"

i32 SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
    i32 offsetx, offsety, d;
    i32 status;

    // CHECK_RENDERER_MAGIC(renderer, -1);

    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y - offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

void Draw(SDL_Renderer* renderer, Position* position, Life* life, SDL_Texture* texture) {
    if (!renderer || !position) {
        return;
    }

    SDL_Rect rectangle = {
        .x = position->X - (position->R),
        .y = position->Y - (position->R),
        .w = position->R * 2,
        .h = position->R * 4,
    };

    u8 shade = life ? (life->Health / life->MaxHealth) * 255 : 255;
    if (texture) {
        SDL_SetTextureColorMod(texture, 255, shade, shade);
        SDL_RenderCopy(renderer, texture, NULL, &rectangle);
    } else {
        // SDL_SetRenderDrawColor(renderer, 255, shade, shade, 255);
        // SDL_RenderFillRect(renderer, &rectangle);
        SDL_RenderDrawCircle(renderer, position->X, position->Y, position->R);
    }
}
