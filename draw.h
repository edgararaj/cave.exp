#pragma once
#include "objects.h"
#include "screen.h"

void bitmap_draw_rectangle(Bitmap bitmap, Rect rect);
void print_triangle(WINDOW *win, int startrow, int startcol, int height);
void print_rectangleu(WINDOW *win, int startrow, int startcol, int height, int width);
void print_ellipse(WINDOW *win, Vec2i, int y, int x, int r1, int r2);
void print_circle(WINDOW *win, Vec2i, Circle);
void print_rectangle(WINDOW *win, Rect rect);