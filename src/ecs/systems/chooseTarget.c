#include <float.h>
#include <stdlib.h>

#include "chooseTarget.h"
#include "../components/position.h"
#include "../components/target.h"
#include "../components/controller.h"
#include "../../types.h"

void ChooseTarget(
        usize entityId,
        Controller* controller,
        Target* target,
        Position** positions,
        usize positionsCount) {
    if (!controller || !target || !positions || !positionsCount) {
        return;
    }

    usize closestEnemyId = entityId;
    f64 closestEnemyDistance = DBL_MAX;
    Position playerPosition = *positions[entityId];
    for (usize i = 0; i < positionsCount; i++) {
        if (i == entityId || !positions[i]) {
            continue;
        }

        f64 distanceX = positions[i]->X - playerPosition.X;
        f64 distanceY = positions[i]->Y - playerPosition.Y;
        f64 distance = distanceX * distanceX + distanceY * distanceY;
        if (distance < closestEnemyDistance) {
            closestEnemyDistance = distance;
            closestEnemyId = i;
        }
    }

    if (closestEnemyId == entityId) {
        return;
    }

    // printf("Enemy entity ID: %zu\n", closestEnemyId);
    target->TargetId = closestEnemyId;
    target->Position = positions[closestEnemyId];
    target->Distance = sqrt(closestEnemyDistance);
}
