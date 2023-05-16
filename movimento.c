#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>
#include "objects.h"

void do_movement_action(Rect *st, int dx, int dy)
{
    st->tl.y += dx;
    st->br.y += dx;
    st->tl.x += dy;
    st->br.x += dy;
}

void update(Rect *st, int key)
{

    switch (key)
    {
    case KEY_A1:
    case '7':
        do_movement_action(st, -1, -1);
        break;
    case KEY_UP:
    case 'w':
        do_movement_action(st, -1, +0);
        break;
    case KEY_A3:
    case '9':
        do_movement_action(st, -1, +1);
        break;
    case KEY_LEFT:
    case 'a':
        do_movement_action(st, +0, -1);
        break;
    case KEY_B2:
    case '5':
        break;
    case KEY_RIGHT:
    case 'd':
        do_movement_action(st, +0, +1);
        break;
    case KEY_C1:
    case '1':
        do_movement_action(st, +1, -1);
        break;
    case KEY_DOWN:
    case 's':
        do_movement_action(st, +1, +0);
        break;
    case KEY_C3:
    case '3':
        do_movement_action(st, +1, +1);
        break;
    default: break;
    }
}
