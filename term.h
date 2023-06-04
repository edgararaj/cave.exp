#pragma once
#include <ncurses.h>
void add_term_line(const char *format, ...);
void render_term(WINDOW *win, int delta_us);