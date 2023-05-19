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

void init_game(GameState *gs, Rect window, WINDOW *win_menu) {
    Rect rects[20];
    int rects_count = generate_rects(expand_rect(window, -5), rects, ARRAY_SIZE(rects));
    Rect ordered_rects[ARRAY_SIZE(rects)];
    order_rects(rects, rects_count);

    for (int i = 0; i < rects_count; i++)
    {
        rects[i].color = 1;
    }

    GameState gs;
    Bitmap pixmap = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
    generate_tunnels_and_rasterize(&gs, pixmap, rects, rects_count);
    erode(pixmap, 2200);

    bitmap_draw_box(pixmap, window);

    uint32_t illuminated_data[MAP_WIDTH][MAP_HEIGHT] = {};
    Bitmap illuminated = {(uint32_t *)illuminated_data, {{MAP_WIDTH, MAP_HEIGHT}}};

    Vec2i first_rect_center = get_center(rects[0]);
    Warrior player;
    player.rect =
        (RectFloat){{first_rect_center.x, first_rect_center.y}, {first_rect_center.x, first_rect_center.y}, 2};
    player.dmg = 5;
    player.hp = 100;
    player.maxHP = 100;
    player.kills = 0;
    player.weight = 5;
    player.velocity = (Vec2f){0, 0};
    player.dmg_cooldown = 0;

    Camera camera = {{{0, 0}}, 0, 0, 10};

    CameraMode cam_mode = CameraMode_Follow;

    Torch torches[MAX_TORCHES];
    create_torches(pixmap, torches, MAX_TORCHES);

    Mob mobs[MAX_MOBS];
    create_mobs(pixmap, mobs, MAX_MOBS);

    Inventory inventory;
    init_inventory(&inventory, 10);
    noecho();

    // Define the items
    Item sword = {ITEM_TYPE_SWORD, "Sword", 'S', COLOR_WHITE};
    Item blastgun = {ITEM_TYPE_BLASTGUN, "Blastgun", 'B', COLOR_WHITE};
    Item health_potion = {ITEM_TYPE_HEALTH_POTION, "Health Potion", 'H', COLOR_WHITE};
    Item coins = {ITEM_TYPE_COINS, "Coins", 'C', COLOR_WHITE};
    Item key = {ITEM_TYPE_KEY, "Key", 'K', COLOR_WHITE};

    // Add the items to the inventory
    add_item(&inventory, sword);
    add_item(&inventory, blastgun);
    for (int i = 0; i < 5; i++)
    {
        add_item(&inventory, health_potion);
    }
    for (int i = 0; i < 20; i++)
    {
        add_item(&inventory, coins);
    }
    add_item(&inventory, key);

    Player_Stats player_stats;
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
    gs.minimap_maximized = false;
    gs.player_stats = player_stats;
    gs.chestCount = 3;
    gs.chests[0] = (Chest){.position = {10, 10}, .isOpen = false, .item = ITEM_GOLD};
    gs.chests[1] = (Chest){.position = {20, 20}, .isOpen = false, .item = ITEM_SWORD};
    gs.chests[2] = (Chest){.position = {30, 30}, .isOpen = false, .item = ITEM_POTION};



    for (int i = 0; i < rects_count; i++)
    {
        generate_spikes(pixmap, rects[i]);
        generate_obstacles(pixmap, rects[i]);
        generate_chests(&gs, pixmap, rects[i]);
    }
    generate_portal(&gs, pixmap, rects[rects_count - 1]);
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

    // WINDOW *win = newwin(30, INGAME_TERM_SIZE, 0, 0);
    WINDOW *win_game = newwin(30, 20, 0, INGAME_TERM_SIZE);
    WINDOW *win_inventory = newwin(30, 20, 0, INGAME_TERM_SIZE);
    WINDOW *win_menu = newwin(30, 20, 0, INGAME_TERM_SIZE);
    WINDOW *win_info = newwin(30, 20, 0, INGAME_TERM_SIZE);
    WINDOW *terminalWin = newwin(30, 20, 0, 0);
    wbkgd(win_game, COLOR_PAIR(Culur_Light_Gradient + LIGHT_RADIUS - 1));

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
    gs.terminalwin = terminalWin;
    gs.player_attacking = 0;
    gs.minimap_maximized = false;
    gs.chestCount = 3;
    gs.chests[0] = (Chest){.position = {10, 10}, .isOpen = false, .item = ITEM_GOLD};
    gs.chests[1] = (Chest){.position = {20, 20}, .isOpen = false, .item = ITEM_SWORD};
    gs.chests[2] = (Chest){.position = {30, 30}, .isOpen = false, .item = ITEM_POTION};

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

    int contador = 0;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    int delta_ms = 0;
    while (1) {
        getmaxyx(stdscr, window_size.y, window_size.x);

        window_size.x -= INGAME_TERM_SIZE;
        // wresize(win, window_size.y, INGAME_TERM_SIZE);
        wresize(win_game, window_size.y, window_size.x);
        window_size.x /= X_SCALE;
        // werase(win);
        werase(win_game);
        wattrset(win_game, COLOR_PAIR(0));

        // add_term_line("%d, %d\n", window_size.x, window_size.y);
        int key = getch();

        if (state == State_Game) {
            draw_game(&gs, window_size, key, &state, delta_ms);
            contador++;
            displayGameWindow(&gs.player_stats);
        } else if (state == State_Menu) {
            draw_menu(&sms, &state, key);
        } else if (state == State_Controlos) {
            draw_controlos(win_info, key, &state, contador);
        } else if (state == State_Niveis) {
            draw_niveis(&smsm, &state, key);
        } else if (state == State_Info) {
            draw_info(win_info, key, &state);
        } else if (state == State_Pause) {
            draw_pause(&smsms, &state, key);
        } else if (state == State_New_Game) {
            state = State_Game;
            init_game(&gs, window, win_menu);
            displayGameWindow(&gs.player_stats);
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        struct timeval result;
        timeval_subtract(&result, (struct timeval*) &end, (struct timeval*) &start);
        delta_ms = result.tv_usec * 1e-4;
        int fps = 1e8 / result.tv_usec;
        start = end;
    }

    endwin();
    return 0;
}
