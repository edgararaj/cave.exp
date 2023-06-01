#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "state.h"
#include "utils.h"

void draw_menu(StartMenuState *sms, State *state, int choice, Vec2i window_size)
{
    wresize(sms->win, window_size.y, window_size.x);
    werase(sms->win);
    char *choices[] = {"START", "CONTROLOS",     "INFO",

                       "QUIT",  "WORK DONE BY:", "Afonso Martins", "Davide Santos", "Edgar Araujo", "Goncalo Barroso"};
    int n_choices = ARRAY_SIZE(choices);

    int max_x = window_size.x;
    int max_y = window_size.y;

    int start_y = (max_y - n_choices) / 2; // Posição vertical inicial do menu

    switch (choice)
    {
    case KEY_UP:
        --sms->highlight;
        if (sms->highlight < 0)
        {
            sms->highlight = n_choices - 6;
        }
        break;
    case KEY_DOWN:
        ++sms->highlight;
        sms->highlight %= 4;
        break;
    default:
        break;
    }

    for (int i = 0; i < n_choices; i++)
    {
        int y = start_y + i;
        if (i == sms->highlight)
        {
            wattron(sms->win, A_REVERSE);
        }

        int text_width = strlen(choices[i]);
        int text_x = (max_x - text_width) / 2;            // Posição horizontal centralizada

        mvwprintw(sms->win, y, text_x, "%s", choices[i]); // +1 para compensar a borda esquerda da box

        wattroff(sms->win, A_REVERSE);
    }

    if (choice == 10)
    {
        if (sms->highlight == 0)
        {
            *state = State_Niveis;
        }
        if (sms->highlight == 1)
        {
            *state = State_Controlos;
        }
        if (sms->highlight == 2)
        {
            *state = State_Info;
        }
        if (sms->highlight == 3)
        {
            exit(0);
        }
    }

    wrefresh(sms->win);
}
