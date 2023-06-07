#pragma once
#include "camera.h"
#include "objects.h"
#include "screen.h"
#include "state.h"

#define MAP_WIDTH (2 * GAME_WIDTH)
#define MAP_HEIGHT (2 * GAME_HEIGHT)

#define WALL 0
#define WALKABLE 1
#define SHINE 2
#define SPIKE 3
#define PORTAL 6
#define OUTER_PORTAL 7

#define SPIKE_DAMAGE 7
#define SPIKE_DAMAGE_COOLDOWN 1e6

int map_is_wall(Bitmap pixmap, Vec2f pos);
void render_map(WINDOW *win_game, Camera camera, Bitmap map, Bitmap light, Bitmap dist, WINDOW *window,
                Bitmap illuminated);
void render_minimap(WINDOW *win, Bitmap illuminated, Vec2i window_size, Vec2i player_pos);
int map_is_walkable(Bitmap pixmap, Vec2f pos, Vec2f inc);
void add_light_map_value(Bitmap bitmap, Vec2i pos, int value);

int generate_rects(Rect window, Rect *rects, int rects_max);
void order_rects(Rect *rects, int rects_count);
void generate_tunnels_and_rasterize(Bitmap bitmap, Rect *rects, int rect_count);
void erode(Bitmap bitmap, int iterations);
void generate_spikes(Bitmap pixmap, Rect rect2);
void generate_obstacles(Bitmap bitmap, Rect rect2);
Chest generate_chest(Rect rect2);
Rect generate_portal(Rect rect2);
void draw_chest(WINDOW *win, Rect rect);
void draw_portal(WINDOW *win, Rect rect);
