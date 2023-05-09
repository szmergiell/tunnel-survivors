#include <math.h>

#include "../components/position.h"
#include "../components/target.h"
#include "../components/velocity.h"
#include "../components/direction.h"
#include "aim.h"

void Aim(Position* position, Target* target, Velocity* worldVelocity, f64 dt) {
    if (!position || !target || !target->Position || !target->Direction || !worldVelocity) {
        return;
    }

    target->Direction->X = target->Position->X + (i32)(worldVelocity->X * dt) - position->X;
    target->Direction->Y = target->Position->Y + (i32)(worldVelocity->Y * dt) - position->Y;

    target->Distance = sqrt(target->Direction->X * target->Direction->X +
                            target->Direction->Y * target->Direction->Y);

    if (target->Distance) {
        target->Direction->X /= target->Distance;
        target->Direction->Y /= target->Distance;
    }
}
