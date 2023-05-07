#include "faceTarget.h"
#include "../components/target.h"
#include "../components/velocity.h"

void FaceTarget(Target* target, Velocity* velocity) {
    if (!target || !target->Direction || !velocity) {
        return;
    }

    velocity->X = target->Direction->X;
    velocity->Y = target->Direction->Y;
}
