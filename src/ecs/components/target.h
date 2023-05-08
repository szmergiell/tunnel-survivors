#pragma once

#include "../../types.h"
#include "direction.h"
#include "position.h"

typedef struct Target {
    usize TargetId;
    Position* Position;
    Direction* Direction;
    f64 Distance;
} Target;
