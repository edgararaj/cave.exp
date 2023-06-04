#include <assert.h>
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define X_SCALE 2
#include "objects.h"
#include "draw.h"
#include "colors.h"
#include "utils.h"

/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.  */

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec)
    {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000)
    {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

int main()
{
    srand(time(NULL));
    cbreak();
    nonl();
    initscr();
    curs_set(0);
    start_color();
    intrflush(stdscr, 0);
    keypad(stdscr, 1);
    nodelay(stdscr, 1);

    WINDOW *win_game = newwin(0, 0, 0, 0);
    Vec2i window_size = (Vec2i){0};
    noecho();
    wbkgd(win_game, COLOR_PAIR(Color_Black));
    setup_colors();

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    int delta_us = 0;
    int i = 0;
    int timer = 0;
    while (1) {
        getmaxyx(stdscr, window_size.y, window_size.x);
        wresize(win_game, window_size.y, window_size.x);
        int key = getch();
        werase(win_game);

        if (timer_update(&timer, delta_us)) {
            timer = 1e6;
            i = (i + 1) % 4;
        }
        wattrset(win_game, COLOR_PAIR(i));
        print_pixel(win_game, 5, 5);

        wattrset(win_game, COLOR_PAIR(Culur_Default));
        mvwprintw(win_game, window_size.y - 1, 0, "Press ESC or P to pause | %d FPS | %f ms", (int)(1e6/delta_us), 1e-3*delta_us);
        wrefresh(win_game);

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        struct timeval result;
        timeval_subtract(&result, (struct timeval*) &end, (struct timeval*) &start);
        delta_us = result.tv_usec + result.tv_sec * 1e6;
        start = end;
    }

    endwin();
    return 0;
}
