#pragma once

#include "../components/position.h"
#include "direction.h"
#include "velocity.h"

void Move(Position* position, Velocity* velocity, Velocity* worldVelocity, f64 dt);
