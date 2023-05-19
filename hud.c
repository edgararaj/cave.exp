#include "player.h" // A presumir que a struct do Player esteja definida neste ficheiro
#include <ncurses.h>

// void displayHUD(Player_Stats *player) {
//     int startRow = 0;
//
//     // Limpar as cinco linhas
//     for (int i = startRow; i < startRow + 5; i++) {
//         move(i, 0);
//         clrtoeol();
//     }
//
//     // Imprimir informações do jogador em cada linha
//     mvprintw(startRow, 20, "HP: %d/%d", player->hp, player->maxHP);
//     mvprintw(startRow + 1, 20, "Mana: %d/%d", player->mana, player->maxMana);
//     mvprintw(startRow + 2, 20, "Level: %d", player->level);
//     mvprintw(startRow + 3, 20, "XP: %d", player->experience);
//     mvprintw(startRow + 4, 20, "Gold: %d", player->gold);
//
//     add_term_line("HP: %d/%d", player->hp, player->maxHP);
//
//     // Atualizar a tela
//     refresh();
// }
//

void displayGameWindow(Player_Stats *player) {

    // Criar janela do mini-terminal
    WINDOW *terminalWin = newwin(LINES, 20, 0, 0);
    box(terminalWin, 0, 0);

    // Imprimir a vida do jogador na janela do mini-terminal
    mvwprintw(terminalWin, 1, 1, "Stats: ");
    mvwprintw(terminalWin, 2, 1, "Vida: %d/%d", player->hp, player->maxHP);
    mvwprintw(terminalWin, 3, 1, "Mana: %d/%d", player->mana, player->maxMana);
    mvwprintw(terminalWin, 4, 1, "Level: %d", player->level);
    mvwprintw(terminalWin, 5, 1, "XP: %d", player->experience);
    mvwprintw(terminalWin, 6, 1, "Gold: %d", player->gold);
    //
    // Atualizar as janelas

    wrefresh(terminalWin);

    // Limpar e restaurar configurações

    //  delwin(terminalWin);
    //   endwin();
    refresh();
}
