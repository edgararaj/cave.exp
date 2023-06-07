#pragma once
#include "dist.h"
#include "inventory.h"
#include "items.h"
#include "objects.h"

#define THREAT_RADIUS 10
#define THREAT_RADIUS_SQR (THREAT_RADIUS * THREAT_RADIUS)
#define VISION_RADIUS MAX_DIST_SHINE
#define VISION_RADIUS_SQR (VISION_RADIUS * VISION_RADIUS)

int warrior_attack(Warrior *a, Warrior *b);
