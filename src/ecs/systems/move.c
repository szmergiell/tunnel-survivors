#include <math.h>
#include <stdio.h>

#include "../components/position.h"
#include "move.h"
#include "types.h"
#include "velocity.h"

void Move(Position* position, Velocity* velocity) {
    if (!position || !velocity) {
        return;
    }

    position->X += velocity->X;
    position->Y += velocity->Y;
}

