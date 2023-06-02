#pragma once
#include "state.h"

#define MAX_ARROWS 10

void update_game(GameState *gs, Vec2i window_size, int key, State *state, int delta_us);
void draw_game(GameState *gs, Vec2i window_size, int);
void render_hotbar(WINDOW *win, Hotbar *hotbar, Vec2i window_size);
void init_game(GameState *gs, Rect window);
int use_key(GameState* gs);
