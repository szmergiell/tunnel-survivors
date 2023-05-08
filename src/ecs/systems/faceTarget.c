#include "faceTarget.h"
#include "../components/target.h"
#include "../components/velocity.h"
#include <math.h>

void FaceTarget(Target* target, Velocity* velocity) {
    if (!target || !target->Direction || !velocity) {
        return;
    }

    velocity->X = target->Direction->X;
    velocity->Y = target->Direction->Y;

    f64 speed = 100;

    f64 currentLength = sqrt(velocity->X * velocity->X + velocity->Y * velocity->Y);
    if (currentLength) {
        velocity->X = velocity->X * speed / currentLength;
        velocity->Y = velocity->Y * speed / currentLength;
    }
}
