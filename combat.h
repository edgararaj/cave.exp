#pragma once
#include "objects.h"
#include "state.h"

typedef struct
{
    RectFloat rect;
    Vec2f velocity;
    int hp;
    int dmg;
    int weight;
    int kills;
} Warrior;