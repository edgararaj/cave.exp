#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

typedef struct {
    int x;
    int y;
} Vec2i;

typedef struct {
    float x;
    float y;
} Vec2f;

typedef struct {
    Vec2i start;
    Vec2i end;
} Line;

typedef struct {
    Vec2i center;
    int radius;
} Circle;

typedef struct {
    Vec2i center;
    Vec2i radius;
} Ellipse;

void print_triangle(WINDOW* win, int startrow, int startcol, int height);
void print_rectangle(WINDOW* win, int startrow, int startcol, int height, int width);
void print_ellipse(WINDOW* win, Vec2i, int y, int x, int r1, int r2);
void print_circle(WINDOW* win, Vec2i, Circle);
int collide_ellipse_line(Ellipse ellipse, Line line);
int collide_circle_line(Circle ellipse, Line line);

typedef struct
{
    char *name;
    char *value;
} Xresource;

char *get_xresource(char *start, char *end)
{
    Xresource resources[] = {
        {"FONTSIZE", "1"},
        {"WIDTH", "800"},
        {"HEIGHT", "200"}};

    for (size_t i = 0; i < ARRAY_SIZE(resources); i++)
    {
        if (strncmp(start, resources[i].name, end - start) == 0)
        {
            return resources[i].value;
        }
    }
    assert(!"Xresource not found");
    return 0;
}

void setup_xresources()
{
    char file[256];
    strcat(strcpy(file, getenv("HOME")), "/.Xresources");
    FILE *w = fopen(file, "w");
    FILE *f = fopen(".Xresources.template", "r");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *temp = malloc(fsize + 1);
    fread(temp, fsize, 1, f);

    char *find = temp;
    while (1)
    {
        char *start = strstr(find, "<!");
        if (start == NULL)
            break;
        char *end = strstr(start, ">");
        char *value = get_xresource(start + 2, end);
        fwrite(find, 1, start - find, w);
        fwrite(value, 1, strlen(value), w);
        find = end + 1;
    }
    fwrite(find, 1, temp + strlen(temp) - find, w);
}

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

int term_line_index = 0;
char term_lines[20][50] = {};
void add_term_line(const char* format, ...) {
if (term_line_index == 19)
{
    for (int i = 0; i < 19; i++)
    {
        strcpy(term_lines[i], term_lines[i+1]);
    }
    term_line_index = 18;
}
  va_list va;
  va_start(va, format);
  const int ret = vsnprintf(term_lines[term_line_index++], 50, format, va);
  va_end(va);
}

void render_term(WINDOW* win) {
  for (int i = 0; i < 20 && *term_lines[i]; i++) {
    mvwprintw(win, i+1, 1, term_lines[i]);
  }
}

Vec2i vec2i_add(Vec2i a, Vec2i b)
{
    Vec2i r = {a.x + b.x, a.y + b.y};
    return r;
}

Vec2i vec2i_mul(Vec2i a, Vec2i b)
{
    Vec2i r = {a.x * b.x, a.y * b.y};
    return r;
}

Vec2i vec2i_mul_const(Vec2i a, int b)
{
    Vec2i r = {a.x * b, a.y * b};
    return r;
}

Vec2i vec2i_add_const(Vec2i a, int b)
{
    Vec2i r = {a.x + b, a.y + b};
    return r;
}

Vec2i vec2i_sub(Vec2i a, Vec2i b)
{
    Vec2i r = {a.x - b.x, a.y - b.y};
    return r;
}

Vec2i vec2i_sub_const(Vec2i a, int b)
{
    Vec2i r = {a.x - b, a.y - b};
    return r;
}

int vec2i_dot(Vec2i a, Vec2i b)
{
    return a.x * b.x + a.y * b.y;
}

int main(int argv, char **argc)
{  
    char* flag = argc[1];
    if (argc[1] && strcmp(argc[1], "--setup") == 0)
    {
        printf("Setting up Xresources\n");
        setup_xresources();
        system("xrdb ~/.Xresources");
        return 0;
    }

    int max_y = 0, max_x = 0;
    time(&fps_timestamp);
    Vec2i vel = {1, 1};
    initscr();
    noecho();
    curs_set(0);
    start_color();
    intrflush(stdscr, 0);
    keypad(stdscr, 1);
    Vec2i window_size;

    WINDOW *win = newwin(30, 20, 0, 0);
    WINDOW *win_game = newwin(30, 20, 0, 20);

    init_pair(1, COLOR_CYAN, COLOR_CYAN);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    wattrset(win, COLOR_PAIR(2));
    wattrset(win_game, COLOR_PAIR(1));
    // init_pair(2, 3, 7);
    // init_pair(3, 4, 2);
    // init_pair(4, 7, 4);
    while (1)
    {
        getmaxyx(stdscr, window_size.y, window_size.x);
        window_size.x -= 20;
        wresize(win_game, window_size.y, window_size.x);
        werase(win);
        werase(win_game);
        wattrset(win_game, COLOR_PAIR(2));
        box(win_game, 0, 0);
        mvwprintw(win_game, 0, 1, "W:%d H:%d", window_size.x, window_size.y);
        wattrset(win_game, COLOR_PAIR(1));
        add_term_line("W:%d H:%d", window_size.x, window_size.y);
        box(win, 0, 0);
        render_term(win);
        // mvwprintw(win, 0, 1, "Console");
        // mvwprintw(win, 1, 1, "Hello");
        Circle circle = {
            {5, 5}, 1
        };
        // add_term_line("x:%d y:%d", x, y);
        mvwprintw(win_game, circle.center.x, circle.center.y, "o");

        circle.center = vec2i_add(circle.center, vel);

        // print_circle(win_game, window_size, circle);
        // char c = getch();
        // if (c == 'w' || c == KEY_UP) 
        //     y -= y_vel;
        // if (c == 's' || c == KEY_DOWN)
        //     y += y_vel;
        // if (c == 'a' || c == KEY_LEFT)
        //     x -= x_vel;
        // if (c == 'd' || c == KEY_RIGHT)
        //     x += x_vel;
        Line window_bottom = {
            {0, window_size.y},
            {window_size.x, window_size.y}
        };
        Line window_left = {
            {0, 0},
            {0, window_size.y}
        };
        Line window_right = {
            {window_size.x, 0},
            {window_size.x, window_size.y}
        };
        Line window_top = {
            {0, 0},
            {window_size.x, 0}
        };

        if (collide_circle_line(circle, window_bottom)) {
            Vec2i normal = {0, 1};
            vel = vec2i_sub(vel, vec2i_mul_const(normal, -2 * vec2i_dot(vel, normal)));
        }
        // else if (collide_circle_line(circle, window_top))
        // {
        //     Vec2i normal = {0, -1};
        // }
        // else if (collide_circle_line(circle, window_left))
        // {
        //     Vec2i normal = {-1, 0};
        // }
        // else if (collide_circle_line(circle, window_right))
        // {
        // }
        // refresh();
        wrefresh(win);
        wrefresh(win_game);
        // limit_fps();
        usleep(100000);
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

int collide_circle_line(Circle circle, Line line)
{
    Ellipse ellipse = {
        circle.center,
        {circle.radius, circle.radius}
    };
    return collide_ellipse_line(ellipse, line);
}
/**
 * ellipse-line collision
 * adapted from http://csharphelper.com/blog/2017/08/calculate-where-a-line-segment-and-an-ellipse-intersect-in-c/
 * @param {number} xe center of ellipse
 * @param {number} ye center of ellipse
 * @param {number} rex radius-x of ellipse
 * @param {number} rey radius-y of ellipse
 * @param {number} x1 first point of line
 * @param {number} y1 first point of line
 * @param {number} x2 second point of line
 * @param {number} y2 second point of line
 */
int collide_ellipse_line(Ellipse ellipse, Line line)
{
    int x1 = line.start.y;
    int x2 = line.end.y;
    int y1 = line.start.x;
    int y2 = line.end.x;
    int xe = ellipse.center.x;
    int ye = ellipse.center.y;
    int rex = ellipse.radius.x;
    int rey = ellipse.radius.y;
    x1 -= xe;
    x2 -= xe;
    y1 -= ye;
    y2 -= ye;

    int A = pow(x2 - x1, 2) / rex / rex + pow(y2 - y1, 2) / rey / rey;
    int B = 2 * x1 * (x2 - x1) / rex / rex + 2 * y1 * (y2 - y1) / rey / rey;
    int C = x1 * x1 / rex / rex + y1 * y1 / rey / rey - 1;
    int D = B * B - 4 * A * C;
    if (D == 0)
    {
        int t = -B / 2 / A;
        return t >= 0 && t <= 1;
    }
    else if (D > 0)
    {
        int sqrt = (int)sqrtf(D);
        int t1 = (-B + sqrt) / 2 / A;
        int t2 = (-B - sqrt) / 2 / A;
        return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
    }
    return 0;
}

void print_circle(WINDOW* win, Vec2i win_size, Circle c)
{
    print_ellipse(win, win_size, c.center.x, c.center.y, c.radius, c.radius);
}

void print_ellipse(WINDOW* win, Vec2i win_size, int x, int y, int r2, int r1)
{
    int d1 = r1 * 2;
    int d2 = r2 * 2;
    int min_y = MAX(y, 1);
    int max_y = MIN(y + d1 + 1, win_size.x);
    for (int i = min_y; i < max_y; i++)
    {
        int min_x = MAX(x, 0);
        int max_x = MIN(x + d2 + 1, win_size.y);
        for (int j = min_x; j < max_x; j++)
        {
            if (sqrt(pow(i - y - r1, 2) / pow(r1, 2) + pow(j - x - r2, 2) / pow(r2, 2)) > 1)
                continue;
            mvwprintw(win, j, i*2, "*");
            mvwprintw(win, j, i*2+1, "*");
        }
    }
}

void print_triangle(WINDOW* win, int startrow, int startcol, int height)
{
    int x = startcol;

    for (int r = startrow; r <= startrow + height; r++)
    {
        for (int c = startcol; c <= x; c++)
        {
            mvwprintw(win, r, c,"*");
        }
        x++;
        startcol--;
    }
}

void print_rectangle(WINDOW* win, int startrow, int startcol, int height, int width)
{
    for (int r = startrow; r <= startrow + height; r++)
    {
        for (int c = startcol; c <= startcol + width; c++)
        {
            mvwprintw(win, r, c, "*");
        }
    }
}
//BOX


//LIGHTING


//MOVEMENT