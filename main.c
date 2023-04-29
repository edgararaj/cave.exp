#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include "objects.h"
#include "utils.h"

// .c
#include "movimento.c"
#include "objects.c"
#include "collide.c"
#include "draw.c"
#include "map.c"
#include "term.c"
#include "xterm.c"
#include "utils.c"

time_t fps_timestamp;
int fps_frame_counter = 0;
int fps = 20;
int fps_limit = 60;
int sleep_time = 10000;

void limit_fps()
{
    time_t current;
    time(&current);
    if (current > fps_timestamp)
    {
        fps = fps_frame_counter;
        fps_frame_counter = 0;
        fps_timestamp = current;
        if (fps < fps_limit)
            sleep_time /= 2;
        if (fps > fps_limit + 2)
            sleep_time += 50 * (fps - fps_limit);
    }
    fps_frame_counter++;
    usleep(sleep_time);
}

int kbhit(void)
{
    int ch = getch();

    if (ch != ERR)
    {
        ungetch(ch);
        return 1;
    }
    else
    {
        return 0;
    }
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
    int max_y = 0, max_x = 0;
    time(&fps_timestamp);
    Vec2i vel = {1, 1};
    cbreak();
    noecho();
    nonl();
    initscr();
    curs_set(0);
    start_color();
    intrflush(stdscr, 0);
    keypad(stdscr, 1);

    Vec2i window_size;

    WINDOW *win = newwin(30, INGAME_TERM_SIZE, 0, 0);
    WINDOW *win_game = newwin(30, 20, 0, INGAME_TERM_SIZE);

    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_CYAN, COLOR_CYAN);
    init_pair(2, COLOR_RED, COLOR_RED);
    wattrset(win, COLOR_PAIR(0));
    wattrset(win_game, COLOR_PAIR(1));
    // init_pair(2, 3, 7);
    // init_pair(3, 4, 2);
    // init_pair(4, 7, 4);
    Rect window = {};
    window.tl.x = 0;
    window.tl.y = 0;
    window.br.x = GAME_WIDTH;
    window.br.y = GAME_HEIGHT;

    Rect rects[20];
    int rects_count = generate_rects(window, rects, ARRAY_SIZE(rects));
    Rect ordered_rects[ARRAY_SIZE(rects)];
    order_rects(rects, rects_count);

    for (int i = 0; i < rects_count; i++)
    {
        rects[i].color = 1;
    }

    int data[GAME_WIDTH][GAME_HEIGHT] = {};
    Bitmap pixmap = {(int *)data, {GAME_WIDTH, GAME_HEIGHT}};
    generate_tunnels_and_rasterize(pixmap, rects, rects_count);
    for (int i = 0; i < 4; i++)
    {
        bitmap_dla_noise(pixmap);
    }

    Vec2i first_rect_center = get_center(rects[0]);
    Rect player = {{first_rect_center.x, first_rect_center.y}, {first_rect_center.x, first_rect_center.y}, 2};

    while (1)
    {
        getmaxyx(stdscr, window_size.y, window_size.x);

        window_size.x -= INGAME_TERM_SIZE;
        wresize(win, window_size.y, INGAME_TERM_SIZE);
        wresize(win_game, window_size.y, window_size.x);
        werase(win);
        werase(win_game);
        wattrset(win_game, COLOR_PAIR(0));
        // mvwprintw(win_game, 0, 1, "W:%d H:%d", window_size.x, window_size.y);
        wattrset(win_game, COLOR_PAIR(1));
        // add_term_line("W:%d H:%d", window_size.x, window_size.y);


        // for (int i = 0; i < rects_count; i++)
        // {
        //     print_rectangle(win_game, rects[i]);
        //     Vec2i center = get_center(rects[i]);
        //     // wattrset(win_game, COLOR_PAIR(0));
        //     // mvwprintw(win_game, center.y, center.x, "%d", rects[i].color);
        // }


        int key = getch();
        add_term_line("%d\n", key);

        wattrset(win_game, COLOR_PAIR(1));
        print_bitmap(win_game, window_size, pixmap);

        // mvwprintw(win, 0, 1, "Console");
        // mvwprintw(win, 1, 1, "Hello");
        // add_term_line("x:%d y:%d", x, y);
        // mvwprintw(win_game, circle.center.y, circle.center.x, "o");

        // add_term_line("x:%d y:%d", x, y);
        // mvwprintw(win_game, circle.center.x, circle.center.y, "o");

        // player = vec2i_add(player, vel);

        Rect prev_player = player;
        update(&player, key);
        if (collide_rect_bitmap(player, pixmap))
        {
            player = prev_player;
        }

        print_rectangle(win_game, player);

        // mvwprintw(win_game, player.y, player.x, "?.");
        // print_circle(win_game, window_size, circle);
        //  char c = getch();
        //  if (c == 'w' || c == KEY_UP)
        //      y -= y_vel;
        //  if (c == 's' || c == KEY_DOWN)
        //      y += y_vel;
        //  if (c == 'a' || c == KEY_LEFT)
        //      x -= x_vel;
        //  if (c == 'd' || c == KEY_RIGHT)
        //      x += x_vel;

        // Line window_bottom = {
        //     {0, window_size.y - 1},
        //     {window_size.x - 1, window_size.y - 1}
        // };
        // Line window_left = {
        //     {0, 0},
        //     {0, window_size.y - 1}
        // };
        // Line window_right = {
        //     {window_size.x - 1, 0},
        //     {window_size.x - 1, window_size.y - 1}
        // };
        // Line window_top = {
        //     {0, 0},
        //     {window_size.x, 0}
        // };

        // Vec2i prev_center = player;
        //  circle.center = vec2i_add(circle.center, vel);

        // if (collide_circle_line(circle, window_bottom)) {
        //     Vec2i normal = {0, 1};
        //     circle.center = prev_center;
        //     vel = vec2i_add(vel, vec2i_mul_const(vec2i_mul_const(normal, vec2i_dot(vel, normal)), -2));
        // Line window_bottom = {
        //     {0, window_size.y},
        //     {window_size.x, window_size.y}};
        // Line window_left = {
        //     {0, 0},
        //     {0, window_size.y}};
        // Line window_right = {
        //     {window_size.x, 0},
        //     {window_size.x, window_size.y}};
        // Line window_top = {
        //     {0, 0},
        //     {window_size.x, 0}};

        // if (collide_circle_line(circle, window_bottom)) {
        //     Vec2i normal = {0, 1};
        //     vel = vec2i_sub(vel, vec2i_mul_const(normal, -2 * vec2i_dot(vel, normal)));
        // }
        // else if (collide_circle_line(circle, window_top))
        // {
        //     Vec2i normal = {0, -1};
        //     circle.center = prev_center;
        //     vel = vec2i_add(vel, vec2i_mul_const(vec2i_mul_const(normal, vec2i_dot(vel, normal)), -2));
        // }
        // else if (collide_circle_line(circle, window_left))
        // {
        //     Vec2i normal = {-1, 0};
        //     circle.center = prev_center;
        //     vel = vec2i_add(vel, vec2i_mul_const(vec2i_mul_const(normal, vec2i_dot(vel, normal)), -2));
        // }
        // else if (collide_circle_line(circle, window_right))
        // {
        //     Vec2i normal = {1, 0};
        //     circle.center = prev_center;
        //     vel = vec2i_add(vel, vec2i_mul_const(vec2i_mul_const(normal, vec2i_dot(vel, normal)), -2));
        // }
        // circle.center = vec2i_add(circle.center, vel);

        // refresh();
        render_term(win);
        box(win, 0, 0);
        wrefresh(win);
        wrefresh(win_game);
        // limit_fps();
        // usleep(100000);
    }

    // attrset(COLOR_PAIR(2));
    // print_triangle(9, 15, 4);

    // attrset(COLOR_PAIR(3));
    // print_triangle(14, 15, 5);

    // attrset(COLOR_PAIR(4));
    // print_rectangle(20, 12, 1, 6);

    endwin();
    return 0;
}
