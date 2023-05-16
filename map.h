#pragma once
#include "screen.h"
#define MAP_WIDTH (2 * GAME_WIDTH)
#define MAP_HEIGHT (2 * GAME_HEIGHT)

#define WALL 0
#define WALKABLE 1
#define SHINE 2
#define DIST_BASE 45
#define MAX_DIST (DIST_BASE + 20)

int map_is_wall(Bitmap pixmap, Vec2f pos);
void render_map(WINDOW *win_game, Camera camera, Bitmap map, WINDOW *window);
void render_minimap(WINDOW *win, Bitmap illuminated, Vec2i window_size,
                    Vec2i player_pos);
int map_is_walkable(Bitmap pixmap, Camera camera, Vec2f pos, Vec2f inc);