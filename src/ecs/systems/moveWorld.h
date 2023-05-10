#pragma once

#include "direction.h"
#include "position.h"
#include "velocity.h"

void MoveWorld(Position* position, Velocity* worldVelocity, f64 dt);
