#include <math.h>
#include <stdlib.h>

#include "map.h"
#include "objects.h"
#include "screen.h"
#include "utils.h"

Vec2i get_center(Rect rect)
{
    Vec2i center;
    center.x = rect.tl.x + (rect.br.x - rect.tl.x) / 2;
    center.y = rect.tl.y + (rect.br.y - rect.tl.y) / 2;
    return center;
}

Vec2f vec2i_to_f(Vec2i a)
{
    Vec2f r = {a.x, a.y};
    return r;
}

Vec2i vec2f_to_i(Vec2f a)
{
    Vec2i r = {a.x, a.y};
    return r;
}

Vec2f vec2f_add(Vec2f a, Vec2f b)
{
    Vec2f r = {a.x + b.x, a.y + b.y};
    return r;
}

Vec2f vec2f_sub(Vec2f a, Vec2f b)
{
    Vec2f r = {a.x - b.x, a.y - b.y};
    return r;
}

Vec2f vec2f_div_const(Vec2f a, int b)
{
    Vec2f r = {a.x / b, a.y / b};
    return r;
}

Vec2i vec2i_add(Vec2i a, Vec2i b)
{
    Vec2i r = {a.x + b.x, a.y + b.y};
    return r;
}

Vec2i vec2i_mul(Vec2i a, Vec2i b)
{
    Vec2i r = {a.x * b.x, a.y * b.y};
    return r;
}

Vec2i vec2i_mul_const(Vec2i a, int b)
{
    Vec2i r = {a.x * b, a.y * b};
    return r;
}

Vec2f vec2f_mul(Vec2f a, Vec2f b)
{
    Vec2f r = {a.x * b.x, a.y * b.y};
    return r;
}

Vec2f vec2f_mul_const(Vec2f a, float b)
{
    Vec2f r = {a.x * b, a.y * b};
    return r;
}

Vec2i vec2i_add_const(Vec2i a, int b)
{
    Vec2i r = {a.x + b, a.y + b};
    return r;
}

Vec2i vec2i_sub(Vec2i a, Vec2i b)
{
    Vec2i r = {a.x - b.x, a.y - b.y};
    return r;
}

Vec2i vec2i_div_const(Vec2i a, int b)
{
    Vec2i r = {a.x / b, a.y / b};
    return r;
}

Vec2i vec2i_sub_const(Vec2i a, int b)
{
    Vec2i r = {a.x - b, a.y - b};
    return r;
}

int vec2i_dot(Vec2i a, Vec2i b)
{
    return (float)a.x * b.x + a.y * b.y;
}

float vec2f_dot(Vec2f a, Vec2f b)
{
    return (float)a.x * b.x + a.y * b.y;
}

float vec2i_sqrdistance(Vec2i a)
{
    return vec2i_dot(a, a);
}

float vec2f_sqrdistance(Vec2f a, Vec2f b)
{
    return vec2f_dot(vec2f_sub(a, b), vec2f_sub(a, b));
}

float vec2f_distance(Vec2f a, Vec2f b)
{
    return sqrtf(vec2f_sqrdistance(a, b));
}

Vec2f vec2f_normalize(Vec2f a)
{
    float len = vec2f_distance(a, (Vec2f){0, 0});
    Vec2f r = {a.x / len, a.y / len};
    return r;
}

Vec2i rect_size(Rect rect)
{
    Vec2i r = {rect.br.x - rect.tl.x + 1, rect.br.y - rect.tl.y + 1};
    return r;
}

Vec2f rect_float_size(RectFloat rect)
{
    Vec2f r = {rect.br.x - rect.tl.x + 1, rect.br.y - rect.tl.y + 1};
    return r;
}

Vec2f rect_center(Rect rect)
{
    Vec2f r = vec2f_add(vec2f_div_const(vec2i_to_f(rect_size(rect)), 2), vec2i_to_f(rect.tl));
    return r;
}

Vec2f rect_float_center(RectFloat rect)
{
    Vec2f r = vec2f_add(vec2f_div_const(rect_float_size(rect), 2), rect.tl);
    return r;
}

Rect rect_translate(Rect rect, Vec2i trans)
{
    Rect result;
    result.color = rect.color;
    result.tl = vec2i_add(rect.tl, trans);
    result.br = vec2i_add(rect.br, trans);
    return result;
}

RectFloat rect_float_translate(RectFloat rect, Vec2f trans)
{
    RectFloat result;
    result.color = rect.color;
    result.tl = vec2f_add(rect.tl, trans);
    result.br = vec2f_add(rect.br, trans);
    return result;
}

Rect rect_float_to_rect(RectFloat rect)
{
    Rect result;
    result.color = rect.color;
    result.tl = vec2f_to_i(rect.tl);
    result.br = vec2f_to_i(rect.br);
    return result;
}

// -------------------
// |        |        |
// |   0    |    1   |
// |        |        |
// -------------------
// |        |        |
// |    2   |    3   |
// |        |        |
// -------------------

Rect subdivide_rect(Rect container, int div, int index)
{
    Rect result;
    Vec2i size = rect_size(container);

    result.tl.x = size.x / div * (index % div);
    result.tl.y = size.y / div * (index / div);
    result.br.x = size.x / div * (((index) % (div)) + 1);
    result.br.y = size.y / div * ((index + div) / (div));
    return rect_translate(result, container.tl);
}

Rect gen_random_subrect(Rect container)
{
    Vec2i size = rect_size(container);
    Rect result;
    int random_width = random_between(size.x / 6, size.x / 2);
    int random_height = random_between(size.y / 6, size.y / 2);
    int random_x = random_between(0, size.x - random_width);
    int random_y = random_between(0, size.y - random_height);
    result.tl.x = container.tl.x + random_x;
    result.tl.y = container.tl.y + random_y;
    result.br.x = container.tl.x + random_x + random_width;
    result.br.y = container.tl.y + random_y + random_height;
    return result;
}

Rect gen_subrect_with_size(Vec2i size, Rect container)
{
    Rect result;
    int random_x = random_between(container.tl.x, container.br.x - size.x - 1);
    int random_y = random_between(container.tl.y, container.br.y - size.y - 1);
    result.tl.x = random_x;
    result.tl.y = random_y;
    result.br.x = random_x + size.x;
    result.br.y = random_y + size.y;
    return result;
}

Rect expand_rect(Rect rect, int amount)
{
    Rect result;
    result.br.y = rect.br.y + amount;
    result.br.x = rect.br.x + amount;
    result.tl.x = rect.tl.x - amount;
    result.tl.y = rect.tl.y - amount;
    return result;
}

void zero_bitmap(Bitmap bitmap)
{
    for (int i = 0; i < bitmap.width * bitmap.height; i++)
    {
        bitmap.data[i] = 0;
    }
}

Bitmap alloc_bitmap(int width, int height)
{
    int *data = (int *)malloc(width * height * sizeof(int));
    Bitmap result = {data, {{width, height}}};
    zero_bitmap(result);
    return result;
}

void free_bitmap(Bitmap bitmap)
{
    free(bitmap.data);
}

void set_bitmap_value(Bitmap bitmap, Vec2i pos, int value)
{
    bitmap.data[pos.y * bitmap.width + pos.x] = value;
}

int get_bitmap_value(Bitmap bitmap, Vec2i pos)
{
    if (pos.x < 0 || pos.x >= bitmap.width || pos.y < 0 || pos.y >= bitmap.height)
    {
        return 0;
    }
    return bitmap.data[pos.y * bitmap.width + pos.x];
}

int get_rect_distance(Bitmap distance, Rect rect)
{
    int tl = get_bitmap_value(distance, rect.tl);
    int tr = get_bitmap_value(distance, (Vec2i){rect.br.x, rect.tl.y});
    int bl = get_bitmap_value(distance, (Vec2i){rect.tl.x, rect.br.y});
    int br = get_bitmap_value(distance, rect.br);
    return MIN(MIN(tl, tr), MIN(bl, br));
}
