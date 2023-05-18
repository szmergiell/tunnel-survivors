#pragma once

#include "life.h"
#include "position.h"
#include "velocity.h"

typedef struct Bullet {
    Position* Start;
    Position* End;
    Velocity* Velocity;
    Life* Life;
} Bullet;
