#pragma once
#include "state.h"

void draw_game(GameState *gs, Vec2i window_size, int key, State *state, int);
void render_hotbar(WINDOW *win, Hotbar *hotbar, Vec2i window_size);
void init_game(GameState *gs, Rect window);
