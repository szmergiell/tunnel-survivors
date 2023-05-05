#pragma once

#include "../components/position.h"
#include "velocity.h"

void Move(Position* position, Velocity* velocity, Position* target);
void MovePlayer(Position* position, Velocity* velocity);
