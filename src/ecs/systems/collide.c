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
            f64 angle = atan2(positions[id]->Y - positions[i]->Y, positions[id]->X - positions[i]->X);
            f64 newX = positions[id]->X;
            f64 newY = positions[id]->Y;
            f64 dX = newX - positions[i]->X;
            f64 dY = newY - positions[i]->Y;
            f64 distance = sqrt(dX * dX + dY * dY);
            f64 sumOfRadii = positions[id]->R + positions[i]->R;
            f64 distanceToMove = sumOfRadii - distance;
            // positions[id]->X += cos(angle) * distanceToMove;
            // positions[id]->Y += sin(angle) * distanceToMove;
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
            f64 angle = atan2(positions[id]->Y - positions[i]->Y, positions[id]->X - positions[i]->X);
            f64 newX = positions[id]->X;
            f64 newY = positions[id]->Y;
            f64 dX = newX - positions[i]->X;
            f64 dY = newY - positions[i]->Y;
            f64 distance = sqrt(dX * dX + dY * dY);
            f64 sumOfRadii = positions[id]->R + positions[i]->R;
            f64 distanceToMove = sumOfRadii - distance;
            // positions[id]->X += cos(angle) * distanceToMove;
            // positions[id]->Y += sin(angle) * distanceToMove;

            if (targets[id]->TargetId == 0 && i == 0) {
                f64 dps = 1;
                f64 damage = dps * dt;
                lives[0]->Health -= damage;
                printf("Attacking enitity ID: %zu with %f damage. Health left: %f\n",
                        targets[id]->TargetId, damage, lives[targets[id]->TargetId]->Health);
            }
            // resolve (?) conflicts so two objects don't get stuck in dead-lock
            // move enemy that is closer to player
            if (i != 0 && targets[id]->Distance < targets[i]->Distance) {
                positions[id]->X += velocities[id]->X * dt;
                positions[id]->Y += velocities[id]->Y * dt;
                velocities[id]->X = velocities[i]->X = 0;
                velocities[id]->Y = velocities[i]->Y = 0;
            } else {
                positions[i]->X += velocities[i]->X * dt;
                positions[i]->Y += velocities[i]->Y * dt;
                velocities[id]->X = velocities[i]->X = 0;
                velocities[id]->Y = velocities[i]->Y = 0;
            }
        }
    }
}
