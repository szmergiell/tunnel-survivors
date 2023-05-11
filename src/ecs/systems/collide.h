#pragma once

#include "../components/position.h"
#include "direction.h"
#include "types.h"
#include "velocity.h"
#include <stdbool.h>

bool WillCollide(Position* position1, Velocity* velocity1, Position* position2, f64 dt);
void CollidePlayer(usize id, Position** positions, Velocity** velocities, usize capacity, f64 dt);
void CollideWorld(usize id, Position** positions, Velocity** velocities, usize capacity, f64 dt);
