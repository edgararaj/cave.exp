#include "collide.h"
#include "map.h"
#include <math.h>

int collide_rect_rect(Rect a, Rect b)
{
    int r1w = rect_size(a).x;
    int r2w = rect_size(b).x;
    int r1h = rect_size(a).y;
    int r2h = rect_size(b).y;
    int r1x = a.tl.x;
    int r2x = b.tl.x;
    int r1y = a.tl.y;
    int r2y = b.tl.y;

    return (r1x + r1w >= r2x && // r1 right edge past r2 left
            r1x <= r2x + r2w && // r1 left edge past r2 right
            r1y + r1h >= r2y && // r1 top edge past r2 bottom
            r1y <= r2y + r2h);  // r1 bottom edge past r2 top
}

int collide_circle_line(Circle circle, Line line)
{
    Ellipse ellipse = {circle.center, {circle.radius, circle.radius}};
    return collide_ellipse_line(ellipse, line);
}

int collide_ellipse_line(Ellipse ellipse, Line line)
{
    int x1 = line.start.y;
    int x2 = line.end.y;
    int y1 = line.start.x;
    int y2 = line.end.x;
    int xe = ellipse.center.y;
    int ye = ellipse.center.x;
    int rex = ellipse.radius.x;
    int rey = ellipse.radius.y;
    x1 -= xe;
    x2 -= xe;
    y1 -= ye;
    y2 -= ye;

    int A = pow(x2 - x1, 2) / rex / rex + pow(y2 - y1, 2) / rey / rey;
    int B = 2 * x1 * (x2 - x1) / rex / rex + 2 * y1 * (y2 - y1) / rey / rey;
    int C = x1 * x1 / rex / rex + y1 * y1 / rey / rey - 1;
    int D = B * B - 4 * A * C;
    if (D == 0)
    {
        int t = -B / 2 / A;
        return t >= 0 && t <= 1;
    }
    else if (D > 0)
    {
        int sqrt = (int)sqrtf(D);
        int t1 = (-B + sqrt) / 2 / A;
        int t2 = (-B - sqrt) / 2 / A;
        return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
    }
    return 0;
}

int collide_line_bitmap(Line line, Bitmap bitmap)
{
    for (int i = line.start.x; i <= line.end.x; i++)
    {
        for (int j = line.start.y; j <= line.end.y; j++)
        {
            if (map_is_wall(bitmap, (Vec2f){i, j}))
                return 1;
        }
    }
    return 0;
}

int collide_rect_bitmap(Rect rect, Bitmap bitmap)
{
    Line left = {{rect.tl.x, rect.tl.y}, {rect.tl.x, rect.br.y}};
    Line top = {{rect.tl.x, rect.tl.y}, {rect.br.x, rect.tl.y}};
    Line right = {{rect.br.x, rect.tl.y}, {rect.br.x, rect.br.y}};
    Line bottom = {{rect.tl.x, rect.br.y}, {rect.br.x, rect.br.y}};

    return (collide_line_bitmap(left, bitmap) || collide_line_bitmap(top, bitmap) ||
            collide_line_bitmap(right, bitmap) || collide_line_bitmap(bottom, bitmap));
}