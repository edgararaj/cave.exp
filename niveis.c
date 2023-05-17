#include "state.h"
#include <ncurses.h>
#include <string.h>

void draw_niveis(StartNiveisState *sms, State *state, int choice) {
    werase(sms->win);
    char *choices[] = {"MODOS:",
                       "FACIL",
                       "MEDIO",
                       "DIFICIL",
                       "MENU",
                       "------------------",
                       "WORK DONE BY:",
                       "Afonso Martins",
                       "Davide Santos",
                       "Edgar Araujo",
                       "Goncalo Barroso"};
    int n_choices = sizeof(choices) / sizeof(char *);

    switch (choice) {
        case KEY_UP:
            --sms->highlight;
            if (sms->highlight < 1) {
                sms->highlight = n_choices - 7;
            }
            break;
        case KEY_DOWN:
            if (sms->highlight == 4)
                sms->highlight = sms->highlight - 4;
            ++sms->highlight;
            sms->highlight %= 5;
            break;
        default:
            break;
    }
    for (int i = 0; i < n_choices; i++) {
        if (i == sms->highlight) {
            wattron(sms->win, A_REVERSE);
        }
        mvwprintw(sms->win, i + 1, 1, "%s", choices[i]);
        wattroff(sms->win, A_REVERSE);
    }
    box(sms->win, 0, 0);

    if (choice == 10) {
        if (sms->highlight == 3) {
            *state = State_Game;
        }
        if (sms->highlight == 4) {
            *state = State_Menu;
            clear();
        }

        mvwprintw(sms->win, 10, 1, "%d", sms->highlight);
    }
    wrefresh(sms->win);
}
