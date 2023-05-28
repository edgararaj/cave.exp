#pragma once
#include "state.h"
#include <ncurses.h>

void draw_menu(StartMenuState *sms, State *state, int choice, Vec2i window_size);
