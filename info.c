#include "state.h"
#include <ncurses.h>
#include <string.h>

#include "state.h"
#include <locale.h>
#include <ncurses.h>
#include <string.h>

void draw_info(WINDOW *win, int key, State *state, Vec2i window_size)
{
    werase(win);
    int width = 35;
    int height = 26;
    int start_y = (window_size.y - height) / 2;
    int start_x = (window_size.x - width) / 2;
    mvwprintw(win, start_y++, start_x, " Bem vindo ao nosso jogo Cave.Exp  ");
    mvwprintw(win, start_y++, start_x, " um jogo onde o objetivo do player ");
    mvwprintw(win, start_y++, start_x, " é procurar chaves escondidas pelo ");
    mvwprintw(win, start_y++, start_x, " mapa inteiro dentro de baús, onde ");
    mvwprintw(win, start_y++, start_x, " estas vão ser precisas para abrir ");
    mvwprintw(win, start_y++, start_x, " portais para niveis subsequentes  ");
    mvwprintw(win, start_y++, start_x, " sendo que em cada nível existente ");
    mvwprintw(win, start_y++, start_x, "  irão ter obstaculos pela frente  ");
    mvwprintw(win, start_y++, start_x, " e terão pela frente, uns temíveis ");
    mvwprintw(win, start_y++, start_x, "  monstros que habitam na caverna  ");
    mvwprintw(win, start_y++, start_x, "      Boa sorte na exploracao      ");
    mvwprintw(win, start_y++, start_x, "     Esperamos que se divirtam!    ");

    if (key == 'q')
    {
        *state = State_Menu;
    }

    wrefresh(win);
}