#pragma once
#include "camera.h"
#include "objects.h"
#include "screen.h"
#define MAP_WIDTH (2 * GAME_WIDTH)
#define MAP_HEIGHT (2 * GAME_HEIGHT)

#define WALL 0
#define WALKABLE 1
#define SHINE 2
#define SPIKE 3
#define SPIKE_DAMAGE 7
#define SPIKE_DAMAGE_COOLDOWN 0.35

#define NORMAL_MAP_SHIFT 0
#define NORMAL_MAP_MASK (0xFF << NORMAL_MAP_SHIFT)

#define DIST_MAP_SHIFT 8
#define DIST_MAP_MASK (0xFF << DIST_MAP_SHIFT)
#define MAX_DIST 20

#define LIGHT_MAP_SHIFT 16
#define LIGHT_MAP_MASK (0xFF << LIGHT_MAP_SHIFT)
#define LIGHT_MAP_MAX LIGHT_RADIUS

#define HIGH_RESOLUTION 2.6
#define DEFAULT_RESOLUTION 9

#define min(a, b) ((a) < (b) ? (a) : (b))

bool minimap_maximized = false;

int map_is_wall(Bitmap pixmap, Vec2f pos);
void render_map(WINDOW *win_game, Camera camera, Bitmap map, WINDOW *window,
                Bitmap illuminated);
void render_minimap(WINDOW *win, Bitmap illuminated, Vec2i window_size,
                    Vec2i player_pos);
int map_is_walkable(Bitmap pixmap, Camera camera, Vec2f pos, Vec2f inc);
void add_light_map_value(Bitmap bitmap, Vec2i pos, int value);

uint32_t dist_map_encode(int value);
uint32_t light_map_encode(int value);
int light_map_decode(uint32_t value);
int dist_map_decode(uint32_t value);
int normal_map_decode(uint32_t value);
uint32_t normal_map_encode(int value);
void set_normal_map_value(Bitmap bitmap, Vec2i pos, int value);
void set_light_map_value(Bitmap bitmap, Vec2i pos, int value);
void set_dist_map_value(Bitmap bitmap, Vec2i pos, int value);
int get_light_map_value(Bitmap bitmap, Vec2i pos);