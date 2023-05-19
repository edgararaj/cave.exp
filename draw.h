#pragma once
#include "objects.h"
#include "inventory.h"
#include "items.h"
#include "screen.h"

void print_pixel(WINDOW *win, int x, int y);
void bitmap_draw_rectangle(Bitmap bitmap, Rect rect);
void print_triangle(WINDOW *win, int startrow, int startcol, int height);
void print_rectangleu(WINDOW *win, int startrow, int startcol, int height, int width);
void print_ellipse(WINDOW *win, Vec2i, int y, int x, int r1, int r2);
void print_circle(WINDOW *win, Vec2i, Circle);
void print_circumference(WINDOW *win, Vec2i win_size, Circle c);
void print_rectangle(WINDOW *win, Rect rect);
void print_line(WINDOW *win, Line line);
void print_pixel_custom(WINDOW *win, int x, int y, char *c);
void print_ellipsumference(WINDOW *win, Vec2i win_size, int x, int y, int r2, int r1);
void bitmap_draw_box(Bitmap bitmap, Rect rect);
void draw_hotbar(WINDOW *win, Inventory *inventory);
