#pragma once
#include "state.h"

#define MAX_ARROWS 10
#define MAX_BLASTS 100
#define BLAST_SPEED 0.001

void update_game(GameState *gs, Vec2i window_size, int key, State *state, int delta_us);
void draw_game(GameState *gs, Vec2i window_size, int);
void player_attack(Mob *mobs, int num_mobs, Warrior *player);
void init_game(GameState *gs, Rect window);
int use_key(GameState *gs);
