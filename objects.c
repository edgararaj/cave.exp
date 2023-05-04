#include "objects.h"
#include "screen.h"
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

Vec2i vec2i_sub_const(Vec2i a, int b)
{
    Vec2i r = {a.x - b, a.y - b};
    return r;
}

float vec2i_dot(Vec2i a, Vec2i b)
{
    return (float)a.x * b.x + a.y * b.y;
}

Vec2i rect_size(Rect rect)
{
    Vec2i r = {rect.br.x - rect.tl.x, rect.br.y - rect.tl.y};
    return r;
}

// -------------------
// |        |        |
// |   0    |    1   |
// |        |        |
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
    return result;
}

Rect gen_random_subrect(Rect container)
{
    Vec2i size = rect_size(container);
    Rect result;
    int random_width = random_between(size.x / 5, size.x / 2);
    int random_height = random_between(size.y / 5, size.y / 2);
    int random_x = random_between(0, size.x - random_width);
    int random_y = random_between(0, size.y - random_height);
    result.tl.x = container.tl.x + random_x;
    result.tl.y = container.tl.y + random_y;
    result.br.x = container.tl.x + random_x + random_width;
    result.br.y = container.tl.y + random_y + random_height;
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