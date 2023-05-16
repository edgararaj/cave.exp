#include "state.h"
#include <ncurses.h>
#include <string.h>

void draw_menu(StartMenuState *sms, State *state, int choice) {
    char *choices[] = {"START",         "CONTINUE",       "TUTORIAL",
                       "WORK DONE BY:", "Afonso Martins", "Davide Santos",
                       "Edgar Araujo",  "Goncalo Barroso"};
    int n_choices = sizeof(choices) / sizeof(char *);

    switch (choice) {
        case KEY_UP:
            --sms->highlight;
            if (sms->highlight < 0) {
                sms->highlight = n_choices - 5 - 1;
            }
            break;
        case KEY_DOWN:
            ++sms->highlight;
            sms->highlight %= 3;
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
        if (sms->highlight == 0) {
            *state = State_Game;
        }
        mvwprintw(sms->win, 10, 1, "%d", sms->highlight);
        if (sms->highlight == 1) {
            *state = State_Info;
        }
    }

    wrefresh(sms->win);
}
