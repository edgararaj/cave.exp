#include <ncurses.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include "objects.h"
#include "utils.h"
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

void render_player(WINDOW *win, Camera camera, Rect player) {
    // Ajusta as coordenadas do jogador para a posição da câmera
    int screen_x = player.tl.x - camera.x;
    int screen_y = player.tl.y - camera.y;
    
    // Renderiza o jogador na posição ajustada
    Rect rect = {
        screen_x, screen_y, player.br.x - camera.x, player.br.y - camera.y
    };
    print_rectangle(win, rect);
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
    nodelay(win_game, 1);

    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_CYAN, COLOR_CYAN);
    init_pair(2, COLOR_RED, COLOR_RED);
    init_pair(3, COLOR_BLUE, COLOR_BLUE);
    wattrset(win, COLOR_PAIR(0));
    wattrset(win_game, COLOR_PAIR(1));
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

    int data[MAP_WIDTH][MAP_HEIGHT] = {};
    Bitmap pixmap = {(int *)data, {MAP_WIDTH, MAP_HEIGHT}};
    generate_tunnels_and_rasterize(pixmap, rects, rects_count);
    erode(pixmap, 4000);
    // for (int i = 0; i < 4; i++)
    // {
    //     bitmap_dla_noise(pixmap);
    // }

    Vec2i first_rect_center = get_center(rects[0]);
    Rect player = {{first_rect_center.x, first_rect_center.y}, {first_rect_center.x, first_rect_center.y}, 2};

    Camera camera = {0, 0, 0, 0, 10};

    while (1)
    {
        getmaxyx(stdscr, window_size.y, window_size.x);

        window_size.x -= INGAME_TERM_SIZE;
        wresize(win, window_size.y, INGAME_TERM_SIZE);
        wresize(win_game, window_size.y, window_size.x);
        werase(win);
        werase(win_game);
        wattrset(win_game, COLOR_PAIR(0));
        wattrset(win_game, COLOR_PAIR(1));

        wattrset(win_game, COLOR_PAIR(0));
        wprintw(win_game, "%d, %d\n", window_size.x, window_size.y);

        camera.width = window_size.x;
        camera.height = window_size.y;
        update_camera(&camera, player.tl.x, player.tl.y);

        int key = getch();

        wattrset(win_game, COLOR_PAIR(1));
        render_map(camera, pixmap, win_game);
        // print_bitmap(win_game, window_size, pixmap);

        Rect prev_player = player;
        update(&player, key);
        if (collide_rect_bitmap(player, pixmap))
        {
            player = prev_player;
        }

        int r = 50;
        float inc = M_PI / 360.f;
        // Atualizar a posição da luz para levar em conta a posição da câmera
        Vec2f light_pos_screen = {
            player.tl.x - camera.x,
            player.tl.y - camera.y};

        wattrset(win_game, COLOR_PAIR(3));
        for (float theta = 0; theta < 2 * M_PI; theta += inc)
        {
            Vec2f vec = {
                cos(theta), sin(theta)};
            Vec2f line_pos = light_pos_screen;
            for (int k = 0; k < r; k++)
            {
                line_pos = vec2f_add(line_pos, vec);

                int data = pixmap.data[(int)(line_pos.y + camera.y) * pixmap.width + (int)(line_pos.x + camera.x)];
                if (!data)
                {
                    break;
                }
                print_pixel(win_game, line_pos.x, line_pos.y);
            }
        }

        wattrset(win_game, COLOR_PAIR(1));
        render_player(win_game, camera, player);
        // print_rectangle(win_game, player);

        render_term(win);
        box(win, 0, 0);

        wrefresh(win);
        wrefresh(win_game);
    }

    endwin();
    return 0;
}
