#pragma once
#include "screen.h"
#define MAP_WIDTH (2*GAME_WIDTH)
#define MAP_HEIGHT (2*GAME_HEIGHT)

#define WALL 0
#define WALKABLE 1
#define SHINE 2
#define DIST_BASE 45
#define MAX_DIST (DIST_BASE+20)

int map_is_wall(Bitmap pixmap, Vec2f pos);