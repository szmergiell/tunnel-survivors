#include "../components/direction.h"
#include "../components/position.h"
#include "../components/velocity.h"
#include "moveWorld.h"

void MoveWorld(Position* position, Velocity* worldVelocity, f64 dt) {
    if (!position || !worldVelocity){
        return;
    }

    position->X -= worldVelocity->X * dt;
    position->Y -= worldVelocity->Y * dt;
}
