#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "state.h"
#include "utils.h"

void draw_game_over(GameOverState *gos, State *state, int choice, Vec2i window_size)
{
    wresize(gos->win, window_size.y, window_size.x);
    werase(gos->win);
    char *choices[] = {"RESTART",       "CONTINUE",       "CONTROLOS",     "INFO",         "QUIT",
                       "WORK DONE BY:", "Afonso Martins", "Davide Santos", "Edgar Araujo", "Goncalo Barroso"};
    int n_choices = sizeof(choices) / sizeof(char *);

    int max_x = window_size.x;
    int max_y = window_size.y;

    int start_y = (max_y - n_choices) / 2; // Posição vertical inicial do menu

    switch (choice)
    {
    case KEY_UP:
        --gos->highlight;
        if (gos->highlight < 0)
        {
            gos->highlight = n_choices - 6;
        }
        break;
    case KEY_DOWN:
        ++gos->highlight;
        gos->highlight %= 5;
        break;
    default:
        break;
    }

    for (int i = 0; i < n_choices; i++)
    {
        int y = start_y + i;
        if (i == gos->highlight)
        {
            wattron(gos->win, A_REVERSE);
        }

        int text_width = strlen(choices[i]);
        int text_x = (max_x - text_width) / 2;            // Posição horizontal centralizada

        mvwprintw(gos->win, y, text_x, "%s", choices[i]); // +1 para compensar a borda esquerda da box

        wattroff(gos->win, A_REVERSE);
    }

    if (choice == 10)
    {
        if (gos->highlight == 0)
        {
            *state = State_New_Game;
        }
        if (gos->highlight == 1)
        {
            *state = State_Game;
        }
        if (gos->highlight == 2)
        {
            *state = State_Controlos;
        }
        if (gos->highlight == 3)
        {
            *state = State_Info;
        }
        if (gos->highlight == 4)
        {
            exit(0);
        }
    }

    wrefresh(gos->win);
}
