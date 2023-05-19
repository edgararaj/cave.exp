#include "state.h"
#include <ncurses.h>
#include <string.h>

#include "state.h"
#include <locale.h>
#include <ncurses.h>
#include <string.h>

void draw_info(WINDOW *win, int key, State *state, int contador) {
    setlocale(LC_ALL, ""); // Configuração para suporte a caracteres amplos
    werase(win);
    mvprintw(20, 102, " -----------------------------------");
    mvprintw(21, 102, "| Bem vindo ao nosso jogo Cave.Exp  |");
    mvprintw(22, 102, "| um jogo onde o objetivo do player |");
    mvprintw(23, 102, "| é procurar chaves escondidas pelo |");
    mvprintw(24, 102, "| mapa inteiro dentro de baús, onde |");
    mvprintw(25, 102, "| estas vão ser precisas para abrir |");
    mvprintw(26, 102, "| portais para niveis subsequentes  |");
    mvprintw(27, 102, "| sendo que em cada nível existente |");
    mvprintw(28, 102, "|  irão ter obstaculos pela frente  |");
    mvprintw(29, 102, "| e terão pela frente, uns temíveis |");
    mvprintw(30, 102, "|  monstros que habitam na caverna  |");
    mvprintw(31, 102, "|      Boa sorte na exploracao      |");
    mvprintw(32, 102, "|     Esperamos que se divirtam!    |");
    mvprintw(33, 102, " -----------------------------------");

    wrefresh(win);
    noecho();

    if (key == 'q') {
        if (contador == 0) {
            *state = State_Menu;
            clear();
        } else
            *state = State_Pause;
        clear();
    }
}