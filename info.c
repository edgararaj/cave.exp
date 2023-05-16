#include "state.h"
#include <ncurses.h>
#include <string.h>
void draw_info(State *state, WINDOW *win, int key) {
    mvprintw(10, 60, " ---------------------------------");
    mvprintw(11, 60, "|           Roguelite             |");
    mvprintw(12, 60, "| Pressione q para voltar ao Menu |");
    mvprintw(13, 60, " --------------------------------- ");

    box(stdscr, 0, 0);
    wrefresh(win);
    noecho();

    if (key == 'q') {
        *state = State_Menu;
        clear();
    }
}