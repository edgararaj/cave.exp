#pragma once
#include "inventory.h"
#include "items.h"
#include "objects.h"

#define THREAT_RADIUS 10
#define THREAT_RADIUS_SQR (THREAT_RADIUS * THREAT_RADIUS)
#define VISION_RADIUS MAX_DIST
#define VISION_RADIUS_SQR (VISION_RADIUS * VISION_RADIUS)

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
    Hotbar hotbar;
} Warrior;

void warrior_attack(Warrior *a, Warrior *b, int delta_ms);