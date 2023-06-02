#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "state.h"
#include "utils.h"
#include "colors.h"

void draw_menu(StartMenuState *sms, State *state, int choice, Vec2i window_size)
{
    wresize(sms->win, window_size.y, window_size.x);
    werase(sms->win);
    char *choices[] = {"START", "CONTROLOS",     "INFO",

                       "QUIT",  "WORK DONE BY:", "Afonso Martins", "Davide Santos", "Edgar Araujo", "Goncalo Barroso"};
    int n_choices = ARRAY_SIZE(choices);

        FILE *f = fopen("name.txt", "r");
    if (f == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    int logo_x = 94;
    int logo_y = 8;

    char n[logo_y][logo_x];
    memset(n, ' ', sizeof(n)); // Inicializa toda a matriz com espaços

    for (int i = 0; i < logo_y; i++) {
        for (int j = 0; j < logo_x; j++) {
            char c = fgetc(f);
            if (c == EOF)
                break;
            if (c == '\n')
                continue; // Ignora os caracteres de nova linha
            n[i][j] = c;
        }
        if (feof(f))
            break;
    }
    fclose(f);

    int y = 4;
    int x = window_size.x / 2 - logo_x / 2;
    for (int i = 0; i < logo_y; i++) {
        for (int j = 0; j < logo_x; j++) {
            mvwprintw(sms->win, y + i, x + j, "%c", n[i][j]);
        }
    }

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

    wbkgd(sms->win, COLOR_PAIR(Culur_Menu));

    for (int i = 0; i < n_choices; i++)
    {
        int y = start_y + i;
        if (i == sms->highlight)
        {
            wattron(sms->win, COLOR_PAIR(Culur_Menu_Highlight)); // Texto branco com fundo cinza
        }
        else
        {
            wattron(sms->win, COLOR_PAIR(Culur_Menu)); // Texto vermelho com fundo preto
        }

        int text_width = strlen(choices[i]);
        int text_x = (max_x - text_width) / 2;

        mvwprintw(sms->win, y, text_x, "%s", choices[i]);

        wattroff(sms->win, COLOR_PAIR(Culur_Menu_Highlight));
        wattroff(sms->win, COLOR_PAIR(Culur_Menu));
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
