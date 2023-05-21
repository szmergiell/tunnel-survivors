#pragma once

#include "life.h"
#include "position.h"
#include "../../sprite.h"
#include "velocity.h"
#include <stdbool.h>

typedef struct Bullet {
    Position* Start;
    Position* End;
    Velocity* Velocity;
    f64 Speed;
    Life* Life;
    f64 LifeDecayPerSecond;
    f64 Damage;
    f64 DamageDecresePerTarget;
    f64 Width;
    bool* EntitiesHit;
    Sprite* Sprite;
} Bullet;
