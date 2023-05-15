#pragma once
#include "objects.h"

typedef enum {
    MobType_Stupid,
    MobType_Coward,
    MobType_Intelligent,
    MobType__Size
} MobType;

typedef struct {
    int dmg;
    int hp;
    float speed;
    MobType type;
    Rect rect;
} Mob;