#pragma once

#include "../components/direction.h"
#include "../components/position.h"
#include "../components/velocity.h"

void MoveWorld(Position* position, Velocity* worldVelocity, f64 dt);
