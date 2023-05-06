#include "../components/position.h"
#include "../components/target.h"
#include "../components/velocity.h"
#include <math.h>

void Aim(Position* position, Target* target, Velocity* velocity) {
    if (!position || !target || !target->Position || !velocity) {
        return;
    }

    f32 directionX = target->Position->X - position->X;
    f32 directionY = target->Position->Y - position->Y;
    f32 magnitude = sqrtf(directionX * directionX + directionY * directionY);
    if (magnitude) {
        directionX /= magnitude;
        directionY /= magnitude;
    }

    velocity->X = directionX;
    velocity->Y = directionY;
}
