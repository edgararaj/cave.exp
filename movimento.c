#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>
#include "state.h"

void do_movement_action(Circle *st, int dx, int dy)
{
    st->center.x += dx;
    st->center.y += dy;
}

void update(Circle *st)
{
    int key = getch();

    switch (key)
    {
    case KEY_A1:
    case '7':
        do_movement_action(st, -1, -1);
        break;
    case KEY_UP:
    case '8':
        do_movement_action(st, -1, +0);
        break;
    case KEY_A3:
    case '9':
        do_movement_action(st, -1, +1);
        break;
    case KEY_LEFT:
    case '4':
        do_movement_action(st, +0, -1);
        break;
    case KEY_B2:
    case '5':
        break;
    case KEY_RIGHT:
    case '6':
        do_movement_action(st, +0, +1);
        break;
    case KEY_C1:
    case '1':
        do_movement_action(st, +1, -1);
        break;
    case KEY_DOWN:
    case '2':
        do_movement_action(st, +1, +0);
        break;
    case KEY_C3:
    case '3':
        do_movement_action(st, +1, +1);
        break;
    case 'q':
        endwin();
        exit(0);
        break;
    }
}
