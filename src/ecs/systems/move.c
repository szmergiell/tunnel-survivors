#include <math.h>
#include <stdio.h>

#include "../components/position.h"
#include "../components/velocity.h"
#include "../components/direction.h"
#include "move.h"

void Move(Position* position, Velocity* velocity, f64 dt) {
    if (!position || !velocity) {
        return;
    }

    position->X += velocity->X * dt;
    position->Y += velocity->Y * dt;
}

