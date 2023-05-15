#pragma once
#include "objects.h"

typedef enum {
    MobType_Normal,
    MobType_Coward,
    MobType_Aggressive,
    MobType__Size
} MobType;

typedef struct {
    int dmg;
    int hp;
    float speed;
    MobType type;
    Rect rect;
} Mob;