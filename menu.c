#include "state.h"
#include <ncurses.h>
#include <string.h>

void draw_menu(StartMenuState *sms, State *state, int choice) {
    werase(sms->win);
    char *choices[] = {"START",         "CONTROLOS",      "INFO",
                       "WORK DONE BY:", "Afonso Martins", "Davide Santos",
                       "Edgar Araujo",  "Goncalo Barroso"};
    int n_choices = sizeof(choices) / sizeof(char *);

    int max_y, max_x;
    getmaxyx(sms->win, max_y, max_x);

    int start_y = (max_y - n_choices) / 2; // Posição vertical inicial do menu

    switch (choice) {
        case KEY_UP:
            --sms->highlight;
            if (sms->highlight < 0) {
                sms->highlight = n_choices - 6;
            }
            break;
        case KEY_DOWN:
            ++sms->highlight;
            sms->highlight %= 3;
            break;
        default:
            break;
    }

    int box_width = max_x - 2;
    int text_width, text_x;

    WINDOW *menuwin = newwin(20, max_x - 6, max_y - 20, 5);
    box(menuwin, 20, 0);

    for (int i = 0; i < n_choices; i++) {
        int y = start_y + i;
        if (i == sms->highlight) {
            wattron(sms->win, A_REVERSE);
        }

        text_width = strlen(choices[i]);
        text_x =
            (box_width - text_width) / 2; // Posição horizontal centralizada

        mvwprintw(sms->win, y, text_x + 1, "%s",
                  choices[i]); // +1 para compensar a borda esquerda da box

        wattroff(sms->win, A_REVERSE);
    }
    // Reposiciona a janela centralizada horizontalmente
    int start_x = (max_x - sms->win->_maxx) / 2;
    mvwin(sms->win, start_y - 1, start_x + 110);

    if (choice == 10) {
        if (sms->highlight == 0) {
            *state = State_Niveis;
        }
        mvwprintw(sms->win, 10, 1, "%d", sms->highlight);
        if (sms->highlight == 1) {
            *state = State_Info;
        }
    }

    wrefresh(sms->win);
}
