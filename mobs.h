#pragma once
#include "combat.h"
#include "objects.h"

#define MAX_MOBS 10

typedef enum
{
    MobType_Stupid,
    MobType_Coward,
    MobType_Intelligent,
    MobType__Size
} MobType;

typedef struct
{
    float speed;
    MobType type;
    Warrior warrior;
} Mob;

void update_mobs(Mob *mobs, int num_mobs, Bitmap map, Warrior *player);