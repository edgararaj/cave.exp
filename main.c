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
#include "inventory.h"

#include "movimento.c"
#include "objects.c"
#include "collide.c"
#include "draw.c"
#include "map.c"
#include "term.c"
#include "xterm.c"
#include "utils.c"
#include "camera.c"
#include "light.c"
#include "inventory.c"

time_t fps_timestamp;
int fps_frame_counter = 0;
int fps = 20;
int fps_limit = 60;
int sleep_time = 10000;
Inventory inventory;

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


// Função para interpolação linear entre duas cores
void lerp_color(short start_color[3], short end_color[3], float t, short *result_color) {
    for (int i = 0; i < 3; i++) {
        result_color[i] = (short) (start_color[i] + t * (end_color[i] - start_color[i]));
    }
}

// Função para inicializar pares de cores com cores de gradiente
void init_gradient_color_pairs(short start_color[3], short end_color[3], int num_pairs, int base) {
    for (int i = base; i < num_pairs; i++) {
        float t = (float) i / (num_pairs - 1);
        short gradient_color[3];
        lerp_color(start_color, end_color, t, gradient_color);

        // Inicializar a cor do gradiente
        init_color(COLOR_PAIR(i + 1), gradient_color[0], gradient_color[1], gradient_color[2]);
        init_pair(i + 1, COLOR_PAIR(i + 1), COLOR_BLACK);
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
    WINDOW *win_inventory = newwin(30, 20, 0, 2 * INGAME_TERM_SIZE);
    noecho();
    nodelay(win_game, 1);

    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_CYAN, COLOR_CYAN);
    init_pair(2, COLOR_RED, COLOR_RED);
    init_pair(3, COLOR_BLUE, COLOR_BLUE);
    init_pair(6, COLOR_YELLOW, COLOR_YELLOW);
    wattrset(win, COLOR_PAIR(0));
    wattrset(win_game, COLOR_PAIR(1));
    Rect window = {};
    window.tl.x = 0;
    window.tl.y = 0;
    window.br.x = GAME_WIDTH;
    window.br.y = GAME_HEIGHT;

    // Definir as cores base para o gradiente (valores entre 0 e 1000)
    short start_color[3] = {0, 0, 0};      // preto
    short end_color[3] = {1000, 1000, 1000}; // branco

    // Número de cores intermediárias para gerar
    int num_pairs = 8;

    // Verificar se o terminal suporta cores e o número mínimo de pares de cores necessários
    if (has_colors() && COLOR_PAIRS >= num_pairs + 1) {
        init_gradient_color_pairs(start_color, end_color, num_pairs, 10);
    } else {
        endwin();
        printf("O terminal não suporta cores ou não tem pares de cores suficientes.\n");
        exit(1);
    }

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
    erode(pixmap, 2200);

    Vec2i first_rect_center = get_center(rects[0]);
    Rect player = {{first_rect_center.x, first_rect_center.y}, {first_rect_center.x, first_rect_center.y}, 2};

    Camera camera = {0, 0, 0, 0, 10};

    CameraMode cam_mode = CameraMode_Margin;


    Rect torches[MAX_TORCHES];
    int num_torches = MAX_TORCHES;
    create_torches(pixmap, torches, num_torches);

    init_inventory(&inventory, 10);

    Item item1 = {"Sword", 'S', COLOR_WHITE};
    Item item2 = {"Potion", 'P', COLOR_RED};
    add_item(&inventory, item1);
    add_item(&inventory, item2);


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
        // wprintw(win_game, "%d, %d\n", window_size.x, window_size.y);
        window_size.x /= X_SCALE;

        camera.width = window_size.x;
        camera.height = window_size.y;
        int key = getch();
        if (key == 't')
        {
            cam_mode = ++cam_mode % CameraMode__Size;
        }

        if (key == ' ')
        {
            add_term_line("ola\n");
            center_camera(&camera, player.tl.x, player.tl.y);
        }

        if (key == 'i') {
            int ch;
            while (1) {
                draw_inventory(win_inventory, &inventory);
                ch = getch();
                if (ch == 'i' || ch == 'q') { // Pressione 'i' ou 'q' para sair do inventário
                    break;
                }
            }
        }


        wattrset(win_game, COLOR_PAIR(1));
        render_map(camera, pixmap, win_game);
        // print_bitmap(win_game, window_size, pixmap);

        Rect prev_player = player;
        update(&player, key);
        if (collide_rect_bitmap(player, pixmap))
        {
            player = prev_player;
        }

        render_light(win_game, camera, pixmap, player.tl.x, player.tl.y, 10);

        for (int i = 0; i < num_torches; i++)
        {
            wattrset(win_game, COLOR_PAIR(6));
            print_pixel(win_game, torches[i].tl.x - camera.x, torches[i].tl.y - camera.y);
            render_light(win_game, camera, pixmap, torches[i].tl.x, torches[i].tl.y, 5);
        }

        wattrset(win_game, COLOR_PAIR(1));
        render_player(win_game, camera, player);
        // print_rectangle(win_game, player);

        if (cam_mode == CameraMode_Margin)
            update_camera(&camera, player.tl.x, player.tl.y);
        else 
            center_camera(&camera, player.tl.x, player.tl.y);

        draw_inventory(win_inventory, &inventory);

        render_term(win);
        box(win, 0, 0);

        wrefresh(win);
        wrefresh(win_game);
    }

    endwin();
    return 0;
}
