#include <math.h>
#include <stdio.h>

#include "../components/position.h"
#include "move.h"
#include "types.h"
#include "velocity.h"

void Move(Position* position, Velocity* velocity, Position* target) {
    if (!position || !velocity || !target) {
        return;
    }

    f32 dx = (target->X - position->X);
    f32 dy = (target->Y - position->Y);
    f32 length = sqrtf(dx * dx + dy * dy);
    if (length) {
        dx /= length;
        dy /= length;
    }

    position->X += dx * velocity->X;
    position->Y += dy * velocity->Y;
}

// TODO: Velocity is used as direction
void MovePlayer(Position* position, Velocity* velocity) {
    if (!position || !velocity) {
        return;
    }

    f32 dx = velocity->X;
    f32 dy = velocity->Y;
    f32 length = sqrtf(dx * dx + dy * dy);
    if (length) {
        dx /= length;
        dy /= length;
    }

    position->X += dx * 1.5;
    position->Y += dy * 1.5;
}
