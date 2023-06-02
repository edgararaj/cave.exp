#include "state.h"
#include <ncurses.h>
#include <string.h>

void draw_niveis(StartNiveisState *sms, State *state, int choice, Vec2i window_size) {
    wresize(sms->win, window_size.y, window_size.x);
    werase(sms->win);
    char *choices[] = {"MODOS:",        "FACIL",          "MEDIO",
                       "DIFICIL",       "BACK",           "------------------",
                       "WORK DONE BY:", "Afonso Martins", "Davide Santos",
                       "Edgar Araujo",  "Goncalo Barroso"};
    int n_choices = sizeof(choices) / sizeof(char *);

    int max_x = window_size.x;
    int max_y = window_size.y;

    int start_y = (max_y - n_choices) / 2; // Posição vertical inicial do menu

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
        int y = start_y + i;
        if (i == sms->highlight) {
            wattron(sms->win, A_REVERSE);
        }

        int text_width = strlen(choices[i]);
        int text_x = (max_x - text_width) / 2; // Posição horizontal centralizada

        mvwprintw(sms->win, y, text_x, "%s", choices[i]); // +1 para compensar a borda esquerda da box

        wattroff(sms->win, A_REVERSE);
    }

    if (choice == 10) {
        if (sms->highlight == 3 || sms->highlight == 2 || sms->highlight == 1) {
            *state = State_Game;
        }
        if (sms->highlight == 4) {
            *state = State_Menu;
        }
    }
    wrefresh(sms->win);
}
