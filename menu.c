#include <ncurses.h>
#include <string.h>

typedef enum {
    GAME,
    MENU
} State;

void desenhar_menu(WINDOW* menuwin, int* highlight, State* state)
{
    char *choices[] = {"START", "CONTINUE", "TUTORIAL", "WORK DONE BY:", "Afonso Martins", "Davide Santos", "Edgar Araujo", "Goncalo Barroso"};
    int n_choices = sizeof(choices) / sizeof(char *);

    for (int i = 0; i < n_choices; i++) {
        if (i == *highlight) {
            wattron(menuwin, A_REVERSE);
        }
        mvwprintw(menuwin, i+1, 1, "%s", choices[i]);
        wattroff(menuwin, A_REVERSE);
    }
    box(menuwin, 0, 0);

    int choice = wgetch(menuwin);
    switch (choice) {
        case KEY_UP:
            --*highlight;
            if (*highlight < 0) {
                *highlight = n_choices - 5- 1;
            }
            break;
        case KEY_DOWN:
            *highlight = ++*highlight % 3;
            break;
        default:
            break;
    }
    if (choice == 10) {
        mvwprintw(menuwin, 10, 1, "%d", *highlight);
    }
}

int main(int argc, char** argv)
{
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    //tamanho do ecra
    int yM, xM;
    getmaxyx(stdscr, yM, xM);

    int width = 50;
    int height = 20;

    //cria uma janela para o nosso output
    WINDOW * game = newwin(0, 0, 0, 0);
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_CYAN, COLOR_CYAN);
    init_pair(2, COLOR_RED, COLOR_RED);
    WINDOW * menuwin = newwin(height, width, (yM - height ) /2, (xM - width) /2);

    //para usar as setas
    keypad(menuwin, true);

    int highlight = 0;

    State state = MENU;

    // input handling loop
    while (1) {
        if (state == MENU)
        {
            desenhar_menu(menuwin, &highlight, &state);
            wrefresh(menuwin);
        }
        else if (state == GAME)
        {
            wrefresh(game);
        }
    }

    endwin();

    return 0;
}
