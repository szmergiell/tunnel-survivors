#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "../components/position.h"
#include "direction.h"
#include "life.h"
#include "move.h"
#include "target.h"
#include "velocity.h"
#include "collide.h"

bool WillCollide(Position* position1, Velocity* velocity1, Position* position2, f64 dt) {
    f64 newX = position1->X + velocity1->X * dt;
    f64 newY = position1->Y + velocity1->Y * dt;
    f64 dX = newX - position2->X;
    f64 dY = newY - position2->Y;
    f64 distance = sqrt(dX * dX + dY * dY);
    f64 sumOfRadii = position1->R + position2->R;

    return distance < sumOfRadii;
}

void CollidePlayer(usize id, Position** positions, Velocity** velocities, usize capacity, f64 dt) {
    if (!positions[id] || !velocities[id]) {
        return;
    }

    for(usize i = 0; i < capacity; i++) {
        if (id == i || !positions[i]) {
            continue;
        }
        if (WillCollide(positions[id], velocities[id], positions[i], dt)) {
            velocities[id]->X = 0;
            velocities[id]->Y = 0;
        }
    }
}

void CollideWorld(
        usize id,
        Position** positions,
        Velocity** velocities,
        Target** targets,
        Life** lives,
        usize capacity, 
        f64 dt) {
    if (!positions[id] || !velocities[id]) {
        return;
    }

    for(usize i = 0; i < capacity; i++) {
        if (id == i || !positions[i]) {
            continue;
        }
        if (WillCollide(positions[id], velocities[id], positions[i], dt)) {
            if (targets[id]->TargetId == 0 && i == 0) {
                f64 dps = 1;
                f64 damage = dps * dt;
                // lives[0]->Health -= damage;
                // printf("Attacking enitity ID: %zu with %f damage. Health left: %f\n",
                //         targets[id]->TargetId, damage, lives[targets[id]->TargetId]->Health);
            }
            // resolve (?) conflicts so two objects don't get stuck in dead-lock
            // move enemy that is closer to player
            if (i == 0) {
                velocities[id]->X = 0;
                velocities[id]->Y = 0;
                continue;
            }
            if (targets[id]->Distance < targets[i]->Distance) {
                velocities[i]->X = 0;
                velocities[i]->Y = 0;
            } else {
                velocities[id]->X = 0;
                velocities[id]->Y = 0;
            }
        }
    }
}

