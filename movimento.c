#include "ncurses.h"
#include "objects.h"

void update_player(RectFloat *st, int key, int delta_us)
{
    Vec2f move = {0};
    switch (key)
    {
    case KEY_A1:
        move = (Vec2f){-1, -1};
        break;
    case KEY_UP:
    case 'w':
        move = (Vec2f){0, -1};
        break;
    case KEY_A3:
        move = (Vec2f){1, -1};
        break;
    case KEY_LEFT:
    case 'a':
        move = (Vec2f){-1, 0};
        break;
    case KEY_RIGHT:
    case 'd':
        move = (Vec2f){1, 0};
        break;
    case KEY_C1:
        move = (Vec2f){-1, 1};
        break;
    case KEY_DOWN:
    case 's':
        move = (Vec2f){0, 1};
        break;
    case KEY_C3:
        move = (Vec2f){1, 1};
        break;
    default:
        break;
    }
    *st = rect_float_translate(*st, move);
}
