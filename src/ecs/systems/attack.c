#include <stdio.h>

#include "../components/target.h"
#include "../components/life.h"

void Attack(Target* target, Life** lives) {
    if (!target || !lives || !target->TargetId) {
        return;
    }

    u32 damage = 1;
    if (lives[target->TargetId] && target->Distance < 400) {
        lives[target->TargetId]->Health -= damage;
        // printf("Attacking enitity ID: %zu. Health left: %u\n", target->TargetId, lives[target->TargetId]->Health);
    }
}
