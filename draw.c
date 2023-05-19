#include "draw.h"
#include "inventory.h"
#include "items.h"
#include "utils.h"
#include <math.h>
#include <ncurses.h>

void print_pixel(WINDOW *win, int x, int y)
{
    print_pixel_custom(win, x, y, "*");
}

void print_pixel_custom(WINDOW *win, int x, int y, char *c)
{
    for (int k = 0; k < X_SCALE; k++)
    {
        mvwprintw(win, y, x * X_SCALE + k, c);
    }
}

void print_circle(WINDOW *win, Vec2i win_size, Circle c)
{
    print_ellipse(win, win_size, c.center.x, c.center.y, c.radius, c.radius);
}

void print_circumference(WINDOW *win, Vec2i win_size, Circle c)
{
    print_ellipsumference(win, win_size, c.center.x, c.center.y, c.radius, c.radius);
}

void print_ellipse(WINDOW *win, Vec2i win_size, int x, int y, int r2, int r1)
{
    int d1 = r1 * 2;
    int d2 = r2 * 2;
    int min_y = MAX(y, 1);
    int max_y = MIN(y + d1 + 1, win_size.x);
    for (int i = min_y; i < max_y; i++)
    {
        int min_x = MAX(x, 0);
        int max_x = MIN(x + d2 + 1, win_size.y * X_SCALE);
        for (int j = min_x; j < max_x; j++)
        {
            if (sqrt(pow(i - y - r1, 2) / pow(r1, 2) + pow(j - x - r2, 2) / pow(r2, 2)) > 1)
                continue;
            print_pixel(win, j, i);
        }
    }
}

void print_ellipsumference(WINDOW *win, Vec2i win_size, int x, int y, int r2, int r1)
{
    int d1 = r1 * 2;
    int d2 = r2 * 2;
    int min_y = MAX(y, 1);
    int max_y = MIN(y + d1 + 1, win_size.x);
    for (int i = min_y; i < max_y; i++)
    {
        int min_x = MAX(x, 0);
        int max_x = MIN(x + d2 + 1, win_size.y * X_SCALE);
        for (int j = min_x; j < max_x; j++)
        {
            float radius = sqrtf(powf(i - y - r1, 2) / powf(r1, 2) + powf(j - x - r2, 2) / powf(r2, 2));
            if (radius > 1 || radius < 0.7f)
                continue;
            print_pixel(win, j, i);
        }
    }
}

void print_bitmap(WINDOW *window, Vec2i window_size, Bitmap bitmap)
{
    int max_y = MIN(bitmap.height, window_size.y);
    for (int y = 0; y < max_y; y++)
    {
        int max_x = MIN(bitmap.width, window_size.x);
        for (int x = 0; x < max_x; x++)
        {
            if (bitmap.data[y * bitmap.width + x] == 1)
            {
                print_pixel(window, x, y);
            }
        }
    }
}

void print_triangle(WINDOW *win, int startrow, int startcol, int height)
{
    int x = startcol;

    for (int r = startrow; r <= startrow + height; r++)
    {
        for (int c = startcol; c <= x; c++)
        {
            mvwprintw(win, r, c, "*");
        }
        x++;
        startcol--;
    }
}

void print_rectangle(WINDOW *win, Rect rect)
{
    wattrset(win, COLOR_PAIR(rect.color));
    print_rectangleu(win, rect.tl.y, rect.tl.x, rect.br.y - rect.tl.y, rect.br.x - rect.tl.x);
}

void print_rectangleu(WINDOW *win, int startrow, int startcol, int height, int width)
{
    for (int r = startrow; r <= startrow + height; r++)
    {
        for (int c = startcol; c <= startcol + width; c++)
        {
            print_pixel(win, c, r);
        }
    }
}

void print_line(WINDOW *win, Line line)
{
    int delta_x = SIGN(line.end.x - line.start.x);
    float delta_y = (float)(line.end.y - line.start.y) / (line.end.x - line.start.x) * SIGN(line.end.x - line.start.x);
    float j = line.start.y;
    for (int i = line.start.x; i != line.end.x; i += delta_x)
    {
        print_pixel(win, i, j);
        j += delta_y;
    }
}

void bitmap_draw_rectangle(Bitmap bitmap, Rect rect)
{
    int startrow = rect.tl.y;
    int startcol = rect.tl.x;
    int height = rect.br.y - rect.tl.y;
    int width = rect.br.x - rect.tl.x;
    for (int r = startrow; r <= startrow + height; r++)
    {
        for (int c = startcol; c <= startcol + width; c++)
        {
            bitmap.data[r * bitmap.width + c] = 1;
        }
    }
}

void bitmap_draw_line(Bitmap bitmap, Line line)
{
    int x0 = line.start.x;
    int y0 = line.start.y;
    int x1 = line.end.x;
    int y1 = line.end.y;
    int dx = ABS(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = ABS(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        bitmap.data[y0 * bitmap.width + x0] = 0;
        if (x0 == x1 && y0 == y1)
            break;
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void bitmap_draw_box(Bitmap bitmap, Rect rect)
{
    Line left = {{rect.tl.x, rect.tl.y}, {rect.tl.x, rect.br.y - 1}};
    Line top = {{rect.tl.x, rect.tl.y}, {rect.br.x - 1, rect.tl.y}};
    Line right = {{rect.br.x - 1, rect.tl.y}, {rect.br.x - 1, rect.br.y - 1}};
    Line bottom = {{rect.tl.x, rect.br.y - 1}, {rect.br.x - 1, rect.br.y - 1}};

    bitmap_draw_line(bitmap, left);
    bitmap_draw_line(bitmap, top);
    bitmap_draw_line(bitmap, right);
    bitmap_draw_line(bitmap, bottom);
}

void draw_hotbar(WINDOW *win, Inventory *inventory)
{
    werase(win);
    wprintw(win, "Hotbar:\n");
    for (int i = 0; i < inventory->size; i++)
    {
        wprintw(win, "%d. %c x%d\n", i + 1, inventory->items[i].symbol, inventory->item_counts[i]);
    }
    wrefresh(win);
}
