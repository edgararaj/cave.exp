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
#include "objects.h"
#include "player.h"
#include "screen.h"
#include "state.h"
#include "utils.h"
#include "xterm.h"

time_t fps_timestamp;
int fps_frame_counter = 0;
int fps = 20;
int fps_limit = 60;
int sleep_time = 10000;
Inventory inventory;
Player_Stats player_stats;
bool minimap_maximized = false;

/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.  */

int timeval_subtract(result, x, y)
struct timeval *result, *x, *y;
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

int main(int argv, char **argc)
{
    char *flag = argc[1];
    if (argc[1] && strcmp(argc[1], "--setup") == 0)
    {
        printf("Setting up Xresources\n");
        setup_xresources();
        system("xrdb ~/.Xresources");
        return 0;
    }

    srand(time(NULL));
    time(&fps_timestamp);
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

    WINDOW *win = newwin(30, INGAME_TERM_SIZE, 0, 0);
    WINDOW *win_game = newwin(30, 20, 0, INGAME_TERM_SIZE);
    WINDOW *win_inventory = newwin(30, 20, 0, INGAME_TERM_SIZE);
    WINDOW *win_menu = newwin(30, 20, 0, INGAME_TERM_SIZE);
    WINDOW *win_info = newwin(30, 20, 0, INGAME_TERM_SIZE);
    wbkgd(win_game, COLOR_PAIR(Culur_Light_Gradient));

    setup_colors();
    wattrset(win, COLOR_PAIR(0));
    wattrset(win_game, COLOR_PAIR(1));

    Rect window = {};
    window.tl.x = 0;
    window.tl.y = 0;
    window.br.x = MAP_WIDTH;
    window.br.y = MAP_HEIGHT;

    Rect rects[20];
    int rects_count = generate_rects(expand_rect(window, -5), rects, ARRAY_SIZE(rects));
    Rect ordered_rects[ARRAY_SIZE(rects)];
    order_rects(rects, rects_count);

    for (int i = 0; i < rects_count; i++)
    {
        rects[i].color = 1;
    }

    Bitmap pixmap = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
    generate_tunnels_and_rasterize(pixmap, rects, rects_count);
    erode(pixmap, 2200);
    for (int i = 0; i < rects_count; i++)
    {
        generate_spikes(pixmap, rects[i]);
        generate_obstacles(pixmap, rects[i]);
    }
    bitmap_draw_box(pixmap, window);

    uint32_t illuminated_data[MAP_WIDTH][MAP_HEIGHT] = {};
    Bitmap illuminated = {(uint32_t *)illuminated_data, {{MAP_WIDTH, MAP_HEIGHT}}};

    Vec2i first_rect_center = get_center(rects[0]);
    Warrior player;
    player.rect =
        (RectFloat){{first_rect_center.x, first_rect_center.y}, {first_rect_center.x, first_rect_center.y}, 2};
    player.dmg = 5;
    player.hp = 100;
    player.kills = 0;
    player.weight = 3;
    player.velocity = (Vec2f){0, 0};

    Camera camera = {{{0, 0}}, 0, 0, 10};

    CameraMode cam_mode = CameraMode_Follow;

    Torch torches[MAX_TORCHES];
    create_torches(pixmap, torches, MAX_TORCHES);

    Mob mobs[MAX_MOBS];
    create_mobs(pixmap, mobs, MAX_MOBS);

    Inventory inventory;
    init_inventory(&inventory, 10);
    noecho();

    //    Item item1 = {"Sword", 'S', COLOR_WHITE};
    //    Item item2 = {"Potion", 'P', COLOR_RED};
    //    add_item(&inventory, item1);
    //    add_item(&inventory, item2);

    player_stats.hp = 100;
    player_stats.maxHP = 100;
    player_stats.mana = 50;
    player_stats.maxMana = 50;
    player_stats.level = 1;
    player_stats.experience = 0;
    player_stats.attackPower = 10;
    player_stats.defense = 5;
    player_stats.speed = 1.0f;
    player_stats.gold = 0;

    GameState gs;
    gs.cam_mode = cam_mode;
    gs.camera = camera;
    gs.player = player;
    gs.torches = torches;
    gs.mobs = mobs;
    gs.pixmap = pixmap;
    gs.win_game = win_game;
    gs.win_inventory = win_inventory;
    gs.illuminated = illuminated;
    gs.inventory = inventory;
    gs.player_attacking = 0;

    State state = State_Menu;

    StartMenuState sms;
    sms.win = win_menu;
    sms.highlight = 0;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    int delta_ms = 0;
    while (1)
    {
        getmaxyx(stdscr, window_size.y, window_size.x);

        window_size.x -= INGAME_TERM_SIZE;
        wresize(win, window_size.y, INGAME_TERM_SIZE);
        wresize(win_game, window_size.y, window_size.x);
        window_size.x /= X_SCALE;
        werase(win);
        werase(win_game);
        wattrset(win_game, COLOR_PAIR(0));

        // add_term_line("%d, %d\n", window_size.x, window_size.y);
        int key = getch();

        if (state == State_Game)
        {
            draw_game(&gs, window_size, key, delta_ms);
            displayHUD(&player_stats);
        }
        else if (state == State_Menu)
        {
            draw_menu(&sms, &state, key);
        }
        else if (state == State_Info)
        {
            draw_info(&state, win_info, key);
        }

        render_term(win);
        box(win, 0, 0);

        wrefresh(win);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        struct timeval result;
        timeval_subtract(&result, &end, &start);
        delta_ms = result.tv_usec * 1e-4;
        int fps = 1e8 / result.tv_usec;
        start = end;

        // add_term_line("%d\n", fps);
    }

    endwin();
    return 0;
}
