#include "state.h"
#include <ncurses.h>
#include <string.h>

#include "state.h"
#include "utils.h"
#include <locale.h>
#include <ncurses.h>
#include <string.h>

static int time_start = 0.2 * 1e6;
static int time = 0.2 * 1e6;
static unsigned long pos = 0;

void draw_text(WINDOW* win, int x, int y, char* c, int delta_us)
{
    char arroz[5024];
    if (timer_update(&time, delta_us)) {
        time = time_start;
        if (pos < strlen(c))
            pos++;
        char* cenoura = strncpy(arroz, c, pos);
        cenoura[pos] = 0;
    }
    mvwprintw(win, y, x, "%s", arroz);
}

void draw_info(WINDOW *win, int key, State *state, Vec2i window_size, int delta_us)
{
    wresize(win, window_size.y, window_size.x);
    setlocale(LC_ALL, ""); // Configuração para suporte a caracteres amplos
    int width = 35;
    int height = 26;
    wresize(win, height, width);
    mvwin(win, (window_size.y - height) / 2, (window_size.x - width) / 2);
    werase(win);
    draw_text(win, 0, 0, "Bem vindo ao nosso jogo Cave.Exp\n"
                         "um jogo onde o objetivo do player\n"
                         "é procurar chaves escondidas pelo\n"
                         "   mapa inteiro dentro de baús,\n"
                         " onde estas vão ser precisas para\n"
                         "   para avancar de nivel sendo\n"
                         "   que em cada nível existente\n"
                         "  irão ter obstaculos pela frente\n"
                         " e terão pela frente, uns temíveis\n"
                         "  monstros que habitam na caverna\n"
                         "     Boa sorte na exploracao\n"
                         "    Esperamos que se divirtam!\n", delta_us);
    if (key == 'q')
    {
        *state = State_Menu;
        mvwin(win, 0, 0);
        pos = 0;
    }

    wrefresh(win);
}
