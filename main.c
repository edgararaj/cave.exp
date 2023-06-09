#include <assert.h>
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "collide.h"
#include "colors.h"
#include "controls.h"
#include "draw.h"
#include "game.h"
#include "hud.h"
#include "info.h"
#include "inventory.h"
#include "items.h"
#include "light.h"
#include "map.h"
#include "menu.h"
#include "mobs.h"
#include "niveis.h"
#include "objects.h"
#include "over.h"
#include "pause.h"
#include "player.h"
#include "screen.h"
#include "state.h"
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

#define BILLION 1000000000L

int main(void)
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

    Vec2i window_size;

    int minimap_height = 20;
    int sidebar_width = minimap_height * X_SCALE;
    int player_stats_height = 10;
    int inventory_height = 10;
    WINDOW *win_game = newwin(30, 20, 0, sidebar_width);
    WINDOW *win_menu = newwin(30, 20, 0, 0);

    WINDOW *win_stats = newwin(player_stats_height, sidebar_width, 0, 0);
    WINDOW *win_inventory = newwin(inventory_height, sidebar_width, player_stats_height, 0);
    WINDOW *win_log = newwin(minimap_height, sidebar_width, player_stats_height + inventory_height, 0);
    WINDOW *win_minimap = newwin(30, sidebar_width, 0, 0);
    wbkgd(win_game, COLOR_PAIR(Culur_Light_Gradient));
    wbkgd(win_minimap, COLOR_PAIR(Culur_Light_Gradient));
    wbkgd(win_log, COLOR_PAIR(Culur_Light_Gradient + 5));
    wbkgd(win_stats, COLOR_PAIR(Culur_Default));

    noecho();

    setup_colors();

    Rect window = {{0, 0}, {0, 0}, 0};
    window.tl.x = 0;
    window.tl.y = 0;
    window.br.x = MAP_WIDTH;
    window.br.y = MAP_HEIGHT;

    GameState gs = {0};
    gs.inventory.items[0].name = Item_Sword;
    gs.inventory.items[0].count = 1;
    gs.inventory.items[0].type = ItemType_Weapon;

    gs.inventory.items[1].name = Item_BlastGun;
    gs.inventory.items[2].name = Item_HealthPotion;
    gs.inventory.items[3].name = Item_ManaPotion;
    gs.inventory.items[4].name = Item_Key;
    gs.inventory.size = 5;

    gs.collision = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
    gs.distance = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
    gs.illuminated = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
    gs.light = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);

    gs.minimap_height = minimap_height;
    gs.sidebar_width = minimap_height;
    gs.player_stats_height = player_stats_height;
    gs.inventory_height = inventory_height;

    gs.win_game = win_game;
    gs.win_inventory = win_inventory;
    gs.win_log = win_log;
    gs.win_minimap = win_minimap;
    gs.win_stats = win_stats;

    StartMenuState sms;
    sms.win = win_menu;
    sms.highlight = 0;

    StartOverState smsmsm;
    smsmsm.win = win_menu;
    smsmsm.highlight = 1;

    StartNiveisState smsm;
    smsm.win = win_menu;
    smsm.highlight = 1;

    StartPauseState smsms;
    smsms.win = win_menu;
    smsms.highlight = 0;

    State state = State_Menu;
    init_game(&gs, window);

    int start_menu = 1;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    int delta_us = 0;
    while (1)
    {
        getmaxyx(stdscr, window_size.y, window_size.x);
        int key = getch();

        if (state == State_Game)
        {
            update_game(&gs, window_size, key, &state, delta_us);
            draw_game(&gs, window_size, delta_us);
        }
        else
        {
            if (state == State_Menu)
            {
                if (start_menu)
                    draw_menu(&sms, &state, key, window_size);
                else
                    draw_pause(&smsms, &state, key, window_size);
            }
            else if (state == State_Controlos)
            {
                draw_controls(win_menu, key, &state, window_size);
            }
            else if (state == State_Niveis)
            {
                draw_niveis(&smsm, &state, key, window_size);
            }
            else if (state == State_Info)
            {
                draw_info(win_menu, key, &state, window_size, delta_us);
            }
            else if (state == State_Pause)
            {
                draw_pause(&smsms, &state, key, window_size);
                start_menu = 0;
            }
            else if (state == State_New_Game)
            {
                state = State_Game;
                init_game(&gs, window);
            }
            else if (state == State_Over)
            {
                draw_game_over(&smsmsm, &state, key, window_size);
            }
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        uint64_t diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
        start = end;
        delta_us = diff * 1e-3;
        // add_term_line("%d ms, %d fps", (int)(diff * 1e-6), (int)(1000/(diff * 1e-6)));
    }

    endwin();
    return 0;
}
