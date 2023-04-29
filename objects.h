#pragma once
#include <ncurses.h>

typedef struct state {
	int playerX;
	int playerY;
} STATE;

typedef struct
{
    int x;
    int y;
} Vec2i;

typedef struct
{
    float x;
    float y;
} Vec2f;

typedef struct
{
    Vec2i start;
    Vec2i end;
} Line;

typedef struct
{
    Vec2i tl;
    Vec2i br;
    short color;
} Rect;

typedef struct
{
    Vec2i center;
    int radius;
} Circle;

typedef struct
{
    Vec2i center;
    Vec2i radius;
} Ellipse;

typedef struct
{
    int *data;
    union
    {
        Vec2i size;
        struct
        {
            int width;
            int height;
        };
    };
} Bitmap;

Rect gen_random_subrect(Rect container);

Vec2i get_center(Rect rect);
Rect expand_rect(Rect rect, int amount);
Vec2i rect_size(Rect rect);
Rect subdivide_rect(Rect container, int div, int index);