#include "state.h"
#include <ncurses.h>
#include <string.h>

#include "state.h"
#include <locale.h>
#include <ncurses.h>
#include <string.h>

void draw_info(WINDOW *win, int key, State *state) {
    setlocale(LC_ALL, ""); // Configuração para suporte a caracteres amplos
    werase(win);
    mvprintw(20, 102, " ---------------------------------");
    mvprintw(21, 102, "|-----------Roguelite--------------|");
    mvprintw(22, 102, "|------q para voltar ao Menu-------|");
    mvprintw(23, 102, "|----------------------------------|");
    mvprintw(24, 102, "|---------Teclas de Atalho---------|");
    mvprintw(25, 102, "|-----KEY_UP - Andar para cima-----|");
    mvprintw(26, 102, "|---KEY_DOWN - Andar para baixo----|");
    mvprintw(27, 102, "|-KEY_LEFT - Andar para a esquerda-|");
    mvprintw(28, 102, "|-KEY_RIGHT - Andar para a direita-|");
    mvprintw(29, 102, "|--------T - Centrar o ecrã--------|");
    mvprintw(30, 102, "|-------M - Abre o mini-mapa-------|");
    mvprintw(31, 102, " --------------------------------- ");

    wrefresh(win);
    noecho();

    if (key == 'q') {
        *state = State_Menu;
        clear();
    }
}
