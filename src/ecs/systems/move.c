#include <math.h>
#include <stdio.h>

#include "../components/position.h"
#include "../components/velocity.h"
#include "direction.h"
#include "move.h"

void Move(Position* position, Velocity* velocity) {
    if (!position || !velocity) {
        return;
    }

    position->X += velocity->X;
    position->Y += velocity->Y;
}

