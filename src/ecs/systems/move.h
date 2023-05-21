#pragma once

#include "../components/position.h"
#include "../components/direction.h"
#include "../components/velocity.h"

void Move(Position* position, Velocity* velocity, f64 dt);
