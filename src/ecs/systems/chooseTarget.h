#pragma once

#include "../../types.h"
#include "../world.h"

// TODO: current ECS system shows its' limitations
// - too many arugments - it would be better to pass world
// - for the firs time world capacity or entity ID matters in system
void ChooseTarget(
        usize entityId,
        Controller* controller,
        Target* target,
        Position** positions,
        usize positionsCount);
