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
    mvprintw(10, 60, " ---------------------------------");
    mvprintw(11, 60, "|-----------Roguelite--------------|");
    mvprintw(12, 60, "|------q para voltar ao Menu-------|");
    mvprintw(13, 60, "|----------------------------------|");
    mvprintw(14, 60, "|---------Teclas de Atalho---------|");
    mvprintw(15, 60, "|-----KEY_UP - Andar para cima-----|");
    mvprintw(16, 60, "|---KEY_DOWN - Andar para baixo----|");
    mvprintw(17, 60, "|-KEY_LEFT - Andar para a esquerda-|");
    mvprintw(18, 60, "|-KEY_RIGHT - Andar para a direita-|");
    mvprintw(19, 60, "|--------T - Centrar o ecrã--------|");
    mvprintw(20, 60, "|-------M - Abre o mini-mapa-------|");
    mvprintw(21, 60, " --------------------------------- ");

    wrefresh(win);
    noecho();

    if (key == 'q') {
        *state = State_Menu;
        clear();
    }
}
