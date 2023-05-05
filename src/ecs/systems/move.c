#include <stdio.h>

#include "../components/position.h"
#include "move.h"

void Move(Position* position, Position* target) {
    if (!position || !target) {
        return;
    }

    // TODO: move towards player (for now it's center of the screen)
    f32 dx = (target->X - position->X)/10000;
    f32 dy = (target->Y - position->Y)/10000;

    position->X += dx;
    position->Y += dy;
}
