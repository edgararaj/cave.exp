#include "state.h"
#include <ncurses.h>
#include <string.h>

void draw_info(State *state, WINDOW *win, int key) {
    werase(win);
    mvprintw(10, 60, " ------------------------------------------------------ ");
    mvprintw(11, 60, "|                       BOOAS!                         |");
    mvprintw(12, 60, "|   Seja bem-vindo ao nosso jogo chamado Cave.Exp, um  |");
    mvprintw(13, 60, "|jogo do subgénero rogue-like endless runner.         |");
    mvprintw(14, 60, "|   Onde o objetivo do player é explorar o mapa que    |");
    mvprintw(15, 60, "|é sempre diferente a cada rejogabilidade, procurar    |");
    mvprintw(16, 60, "|chaves que se encontram escondidas em baús que vão    |");
    mvprintw(17, 60, "| ser precisas para abrir portaís para niveis seguintes|");                 
    mvprintw(18, 60, "| e derrotar monstros que habitam na escura caverna.   |");    
    mvprintw(19, 60, "|    Boa sorte na sua exploração e coragem!          |");
    mvprintw(20, 60, "|     Espero que se divirta com o nosso jogo :)        |");
    mvprintw(21, 60, "|         (Pressione q para voltar ao Menu)            |");
    mvprintw(22, 60, " ------------------------------------------------------ ");

    mvprintw(30, 20, " -------------------------- ");
    mvprintw(31, 20, "|Feito por:                |");
    mvprintw(32, 20, "|Afonso Martins  - A102940 |");
    mvprintw(33, 20, "|Davide Santos   - A102938 |");
    mvprintw(34, 20, "|Edgar Araújo    - A102946 |");
    mvprintw(35, 20, "|Gonçalo Barroso - A102940 |");
    mvprintw(36, 20, " --------------------------");



    

    

    
    wrefresh(win);
    noecho();

    if (key == 'q' || key == 'Q') {
        *state = State_Menu;
        clear();
    }
}