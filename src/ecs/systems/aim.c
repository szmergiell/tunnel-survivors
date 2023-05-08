#include <math.h>

#include "../components/position.h"
#include "../components/target.h"
#include "../components/velocity.h"
#include "../components/direction.h"
#include "aim.h"

void Aim(Position* position, Target* target) {
    if (!position || !target || !target->Position || !target->Direction) {
        return;
    }

    target->Direction->X = target->Position->X - position->X;
    target->Direction->Y = target->Position->Y - position->Y;
    target->Distance = sqrt(target->Direction->X * target->Direction->X +
                            target->Direction->Y * target->Direction->Y);
    if (target->Distance) {
        target->Direction->X /= target->Distance;
        target->Direction->Y /= target->Distance;
    }
}
