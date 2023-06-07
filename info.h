#pragma once

void draw_text(WINDOW *win, int y, char *c, int delta_us);
void draw_info(WINDOW *win, int key, State *state, Vec2i window_size, int);
