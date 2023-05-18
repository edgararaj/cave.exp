#pragma once
#include "objects.h"

typedef struct
{
    RectFloat rect;
    Vec2f velocity;
    int hp;
    int maxHP;
    int dmg;
    int weight;
    int kills;
    int dmg_cooldown;
} Warrior;

void warrior_attack(Warrior *a, Warrior *b, int delta_ms);