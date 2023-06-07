#include "state.h"
#include <ncurses.h>
#include <string.h>

#include "state.h"
#include "utils.h"
#include <locale.h>
#include <ncurses.h>
#include <string.h>

static int time_start = 5e2;
static int time = 5e2;
static unsigned long pos = 0;

void draw_text(WINDOW *win, int y, char *c, int delta_us)
{
    char arroz[5024];
    if (timer_update(&time, delta_us))
    {
        time = time_start;
        if (pos < strlen(c))
            pos++;
        char *cenoura = strncpy(arroz, c, pos);
        cenoura[pos] = 0;
    }
    int max_x = getmaxx(win);
    int text_width = strlen(arroz);
    int text_x = (max_x - text_width) / 2; // Posição horizontal centralizada
    mvwprintw(win, y, text_x, "%s", arroz);
}

void draw_info(WINDOW *win, int key, State *state, Vec2i window_size, int delta_us)
{
    wresize(win, window_size.y, window_size.x);
    setlocale(LC_ALL, ""); // Configuração para suporte a caracteres amplos
    werase(win);
    char *lines[] = {
        "Bem vindo ao nosso jogo Cave.Exp",  "um jogo onde o objetivo do player", "é procurar chaves escondidas pelo",
        "   mapa inteiro dentro de baús,",   " onde estas vão ser precisas para", "   para avancar de nivel sendo",
        "   que em cada nível existente",    "  irão ter obstaculos pela frente", " e terão pela frente, uns temíveis",
        "  monstros que habitam na caverna", "     Boa sorte na exploracao",      "    Esperamos que se divirtam!"};
    int n_lines = sizeof(lines) / sizeof(char *);

    int max_y = getmaxy(win);

    int start_y = (max_y - n_lines) / 2; // Posição vertical inicial do texto

    for (int i = 0; i < n_lines; i++)
    {
        int y = start_y + i;
        draw_text(win, y, lines[i], delta_us);
    }

    if (key == 'q')
    {
        *state = State_Menu;
        mvwin(win, 0, 0);
        pos = 0; // Reseta a posiçao quando saimos da pagina
    }

    wrefresh(win);
}
