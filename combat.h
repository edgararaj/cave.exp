#pragma once
#include "inventory.h"
#include "items.h"
#include "objects.h"

#define THREAT_RADIUS 10
#define THREAT_RADIUS_SQR (THREAT_RADIUS * THREAT_RADIUS)
#define VISION_RADIUS MAX_DIST
#define VISION_RADIUS_SQR (VISION_RADIUS * VISION_RADIUS)

void warrior_attack(Warrior *a, Warrior *b, int delta_ms);