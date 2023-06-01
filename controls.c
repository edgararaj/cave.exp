#include "state.h"
#include <ncurses.h>
#include <string.h>

#include "state.h"
#include <locale.h>
#include <ncurses.h>
#include <string.h>

void draw_controls(WINDOW *win, int key, State *state, Vec2i window_size)
{
    wresize(win, window_size.y, window_size.x);
    werase(win);
    int width = 34;
    int height = 35;
    int start_y = (window_size.y - height) / 2;
    int start_x = (window_size.x - width) / 2;
    mvwprintw(win, start_y++, start_x, "                                    ");
    mvwprintw(win, start_y++, start_x, "             Roguelite              ");
    mvwprintw(win, start_y++, start_x, "       (q para voltar ao Menu)      ");
    mvwprintw(win, start_y++, start_x, "                                    ");
    mvwprintw(win, start_y++, start_x, "          Teclas de Atalho:         ");
    mvwprintw(win, start_y++, start_x, "      KEY_UP   Andar para cima      ");
    mvwprintw(win, start_y++, start_x, "    KEY_DOWN   Andar para baixo     ");
    mvwprintw(win, start_y++, start_x, "  KEY_LEFT   Andar para a esquerda  ");
    mvwprintw(win, start_y++, start_x, "  KEY_RIGHT   Andar para a direita  ");
    mvwprintw(win, start_y++, start_x, "                                    ");
    mvwprintw(win, start_y++, start_x, "         T   Centrar o ecrã         ");
    mvwprintw(win, start_y++, start_x, "        M   Abre o mini mapa        ");
    mvwprintw(win, start_y++, start_x, "          ESC   Abre o menu         ");
    mvwprintw(win, start_y++, start_x, "                                    ");

    if (key == 'q')
    {
        *state = State_Menu;
    }

    wrefresh(win);
}
