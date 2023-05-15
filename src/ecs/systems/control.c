#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "../components/controller.h"
#include "../components/velocity.h"
#include "direction.h"
#include "types.h"
#include "control.h"

// TODO: Velocity is used as direction
void Control(Controller* controller, Velocity* velocity) {
    if (!controller || !velocity) {
        return;
    }

    const u8* keyboardState = SDL_GetKeyboardState(NULL);
    velocity->X = 0;
    velocity->Y = 0;

    if (keyboardState[SDL_SCANCODE_UP] ||
        keyboardState[SDL_SCANCODE_W] ||
        keyboardState[SDL_SCANCODE_K]) {
        velocity->Y += -1;
    }
    if (keyboardState[SDL_SCANCODE_DOWN] ||
        keyboardState[SDL_SCANCODE_S] ||
        keyboardState[SDL_SCANCODE_J]) {
        velocity->Y += 1;
    }
    if (keyboardState[SDL_SCANCODE_LEFT] ||
        keyboardState[SDL_SCANCODE_A] ||
        keyboardState[SDL_SCANCODE_H]) {
        velocity->X += -1;
    }
    if (keyboardState[SDL_SCANCODE_RIGHT] ||
        keyboardState[SDL_SCANCODE_D] ||
        keyboardState[SDL_SCANCODE_L]) {
        velocity->X += 1;
    }

    f64 speed = 150;

    f64 currentLength = sqrt(velocity->X * velocity->X + velocity->Y * velocity->Y);
    if (currentLength) {
        velocity->X = velocity->X * speed / currentLength;
        velocity->Y = velocity->Y * speed / currentLength;
    }
}
