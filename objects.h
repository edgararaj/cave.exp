#pragma once
#include <stdint.h>

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
    Vec2f tl;
    Vec2f br;
    short color;
} RectFloat;

typedef struct
{
    RectFloat rect;
    Vec2f velocity;
} Arrow;

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
    union {
        Vec2i size;
        struct
        {
            int width;
            int height;
        };
    };
} Bitmap;

Rect gen_random_subrect(Rect container);
Rect gen_subrect_with_size(Vec2i size, Rect container);
Vec2i get_center(Rect rect);
Rect expand_rect(Rect rect, int amount);
Vec2i rect_size(Rect rect);
Vec2f rect_float_size(RectFloat rect);
Vec2f rect_center(Rect rect);
Vec2f rect_float_center(RectFloat rect);
Rect subdivide_rect(Rect container, int div, int index);
Rect rect_translate(Rect rect, Vec2i trans);
RectFloat rect_float_translate(RectFloat rect, Vec2f trans);
Vec2f vec2i_to_f(Vec2i a);
Vec2i vec2i_div_const(Vec2i a, int b);
Vec2f vec2f_div_const(Vec2f a, int b);
Vec2f vec2f_mul_const(Vec2f a, float b);
Vec2i vec2f_to_i(Vec2f a);
Vec2f vec2f_add(Vec2f a, Vec2f b);
Vec2f vec2f_sub(Vec2f a, Vec2f b);
Vec2i vec2i_add(Vec2i a, Vec2i b);
float vec2f_dot(Vec2f a, Vec2f b);
float vec2i_sqrdistance(Vec2i a);
float vec2f_sqrdistance(Vec2f a, Vec2f b);
float vec2f_distance(Vec2f a, Vec2f b);
Vec2f vec2f_normalize(Vec2f a);
Vec2i vec2i_add(Vec2i a, Vec2i b);
Vec2i vec2i_sub(Vec2i a, Vec2i b);
Vec2i vec2i_mul_const(Vec2i a, int b);
Bitmap alloc_bitmap(int width, int height);
void free_bitmap(Bitmap bitmap);
int vec2i_dot(Vec2i a, Vec2i b);
Rect rect_float_to_rect(RectFloat rect);
Vec2f vec2f_mul(Vec2f a, Vec2f b);

int get_bitmap_value(Bitmap bitmap, Vec2i pos);
void set_bitmap_value(Bitmap bitmap, Vec2i pos, int value);
void zero_bitmap(Bitmap bitmap);
int get_rect_distance(Bitmap distance, Rect rect);
