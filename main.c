#include <assert.h>
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "collide.h"
#include "colors.h"
#include "draw.h"
#include "game.h"
#include "hud.h"
#include "inventory.h"
#include "items.h"
#include "light.h"
#include "map.h"
#include "menu.h"
#include "mobs.h"
#include "objects.h"
#include "player.h"
#include "screen.h"
#include "state.h"
#include "utils.h"
#include "menu.h"
#include "info.h"
#include "controls.h"
#include "niveis.h"
#include "pause.h"

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
    noecho();
    nonl();
    initscr();
    curs_set(0);
    start_color();
    intrflush(stdscr, 0);
    keypad(stdscr, 1);
    nodelay(stdscr, 1);

    Vec2i window_size;

    int minimap_height = 20;
    int sidebar_width = minimap_height * X_SCALE;
    WINDOW *win_game = newwin(30, 20, 0, sidebar_width);
    WINDOW *win_inventory = newwin(30, 20, 0, 0);
    WINDOW *win_minimap = newwin(30, sidebar_width, 0, 0);
    WINDOW *win_log = newwin(minimap_height, sidebar_width, 0, 0);
    WINDOW *win_menu = newwin(30, 20, 0, 0);
    WINDOW *terminalWin = newwin(30, 20, 0, 0);
    wbkgd(win_game, COLOR_PAIR(Culur_Light_Gradient));
    wbkgd(win_minimap, COLOR_PAIR(Culur_Light_Gradient + 10));
    wbkgd(win_log, COLOR_PAIR(Culur_Light_Gradient + 5));

    setup_colors();
    // wattrset(win, COLOR_PAIR(0));
    wattrset(win_game, COLOR_PAIR(1));

    Rect window = {};
    window.tl.x = 0;
    window.tl.y = 0;
    window.br.x = MAP_WIDTH;
    window.br.y = MAP_HEIGHT;

    GameState gs;
    gs.win_game = win_game;
    gs.win_inventory = win_inventory;
    gs.win_log = win_log;
    gs.win_minimap = win_minimap;
    gs.terminalwin = terminalWin;

    StartMenuState sms;
    sms.win = win_menu;
    sms.highlight = 0;

    StartNiveisState smsm;
    smsm.win = win_menu;
    smsm.highlight = 1;

    StartPauseState smsms;
    smsms.win = win_menu;
    smsms.highlight = 0;

    State state = State_Menu;
    init_game(&gs, window, win_menu);

    int start_menu = 1;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    int delta_us = 0;
    while (1) {
        getmaxyx(stdscr, window_size.y, window_size.x);

        wresize(win_menu, window_size.y, window_size.x);
        // window_size.x -= INGAME_TERM_SIZE;
        // wresize(win_game, window_size.y, window_size.x);
        // wresize(win_menu, window_size.y, window_size.x);
        // window_size.x /= X_SCALE;
        // werase(win_game);
        // wattrset(win_game, COLOR_PAIR(0));

        int key = getch();

        if (state == State_Game) {
            draw_game(&gs, window_size, key, &state, delta_us);
        } else if (state == State_Menu) {
            if (start_menu)
                draw_menu(&sms, &state, key, window_size);
            else 
                draw_pause(&smsms, &state, key, window_size);
        } else if (state == State_Controlos) {
            draw_controls(win_menu, key, &state, window_size);
        } else if (state == State_Niveis) {
            draw_niveis(&smsm, &state, key, window_size);
        } else if (state == State_Info) {
            draw_info(win_menu, key, &state, window_size);
        } else if (state == State_Pause) {
            draw_pause(&smsms, &state, key, window_size);
            start_menu = 0;
        } else if (state == State_New_Game) {
            state = State_Game;
            init_game(&gs, window, win_menu);
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        struct timeval result;
        timeval_subtract(&result, (struct timeval*) &end, (struct timeval*) &start);
        delta_us = result.tv_usec * 1e-1;
        int fps = 1e8 / result.tv_usec;
        start = end;
    }

    endwin();
    return 0;
}
