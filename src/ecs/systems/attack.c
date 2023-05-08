#include <stdio.h>

#include "../components/target.h"
#include "../components/life.h"

void Attack(Target* target, Life** lives, f64 dt) {
    if (!target || !lives || !target->TargetId) {
        return;
    }

    f64 dps = 1.0;
    if (lives[target->TargetId] && target->Distance < 400.0) {
        f64 damage = dps * dt;
        lives[target->TargetId]->Health -= damage;
        // printf("Attacking enitity ID: %zu with %f damage. Health left: %f\n",
        //         target->TargetId, damage, lives[target->TargetId]->Health);
    }
}
