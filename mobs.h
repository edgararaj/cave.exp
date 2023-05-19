#pragma once
#include "combat.h"
#include "objects.h"
#include "state.h"

void update_mob(Mob *mob, Bitmap map, Warrior *player, int delta_ms);
void update_mobs(Mob *mobs, int num_mobs, Bitmap map, Warrior *player, int delta_ms);
void create_mobs(Bitmap pixmap, Mob *mobs, int num_mobs);
