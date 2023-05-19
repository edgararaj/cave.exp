#include "state.h"
#include <ncurses.h>
#include <string.h>

void draw_niveis(StartNiveisState *sms, State *state, int choice) {
    werase(sms->win);
    char *choices[] = {"MODOS:",        "FACIL",          "MEDIO",
                       "DIFICIL",       "MENU",           "------------------",
                       "WORK DONE BY:", "Afonso Martins", "Davide Santos",
                       "Edgar Araujo",  "Goncalo Barroso"};
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

    int max_y, max_x;
    getmaxyx(sms->win, max_y, max_x);
    WINDOW *menuwin = newwin(20, max_x - 6, max_y - 20, 5);
    box(menuwin, 20, 0);
    int box_width = max_x - 2;
    int text_width, text_x;
    int start_y = (max_y - n_choices) / 2; // Posição vertical inicial do menu

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

    if (choice == 10) {
        if (sms->highlight == 3 || sms->highlight == 2 || sms->highlight == 1) {
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
