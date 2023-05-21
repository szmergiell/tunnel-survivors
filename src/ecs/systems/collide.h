#pragma once

#include "../components/position.h"
#include "../components/direction.h"
#include "../components/life.h"
#include "../components/target.h"
#include "../../types.h"
#include "../components/velocity.h"
#include <stdbool.h>

bool WillCollide(Position* position1, Velocity* velocity1, Position* position2, f64 dt);
void CollidePlayer(usize id, Position** positions, Velocity** velocities, usize capacity, f64 dt);
void CollideWorld(
        usize id,
        Position** positions,
        Velocity** velocities,
        Target** targets,
        Life** lives,
        usize capacity,
        f64 dt);
