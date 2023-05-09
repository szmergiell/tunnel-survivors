#include <math.h>
#include <stdio.h>

#include "../components/position.h"
#include "../components/velocity.h"
#include "direction.h"
#include "move.h"

void Move(Position* position, Velocity* velocity, Velocity* worldVelocity, f64 dt) {
    if (!position || !velocity || !worldVelocity) {
        return;
    }

    position->X += velocity->X * dt - (i32)(worldVelocity->X * dt);
    position->Y += velocity->Y * dt - (i32)(worldVelocity->Y * dt);
}

