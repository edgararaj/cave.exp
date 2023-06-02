#pragma once
#include "combat.h"
#include "objects.h"
#include "state.h"

void update_mob(Mob *mobs, int num_mobs, int ii, Bitmap map, Warrior *player, Bitmap player_light, int delta_us);
void update_mobs(Mob *mobs, int num_mobs, Bitmap map, Warrior *player, Bitmap player_light, int delta_us, GameState *gs);
void create_mobs(Bitmap pixmap, Mob *mobs, int num_mobs);
