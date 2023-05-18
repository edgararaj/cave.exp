#pragma once
#include <sys/time.h>

typedef struct
{
    int hp;
    int maxHP;
    int mana;
    int maxMana;
    int level;
    int experience;
    int attackPower;
    int defense;
    float speed;
    int gold;
    struct timeval lastDamageTime;
} Player_Stats;
