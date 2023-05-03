
#pragma once
#include <ncurses.h>
#include "state.h"

WINDOW* menu_create_window();
void draw_menu(WINDOW* menuwin, int* highlight, State* state, Vec2i window_size);