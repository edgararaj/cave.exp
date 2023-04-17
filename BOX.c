#include<ncurses.h>

int BOX(int argc, char **argv)
{
    initscr();

    cbreak();//dentro do programa clicamos ctrl+c para dar exit ao programa de ncurses
    //ja tem no main so meti para me relembrar

    //noecho();dentro do terminal se pressionarmos um carater nao é printado para o terminal se pressionarmos outra vez ele sai do programa de ncurses
    
    //raw();pega em todo o input como raw input e nao aceita carateres especiais como o ctrl+c para sair 

    
 

    int alt, comp, sy, sx; 

    alt = 30;//a janela criada no terminal vai ter 30 unidades de altura
    comp = 50;//a janela criada no terminal vai ter 50 unidades de comprimento
    sx = sy = 20;

    WINDOW * win = newwin(alt, comp, sy,sx);//cria uma janela dentro do terminal;

    char v = '#';
    box(win,(int)v , (int)v);

    wprintw(win, "LA PROJECT");//no extremo superior esquerdo escreve a string escrita
    wrefresh(win);

    int c = getch();

    endwin();

    return 0;
}
