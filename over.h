#pragma once
#include "state.h"
#include <ncurses.h>

void draw_game_over(StartOverState *sms, State *state, int choice, Vec2i window_size);
