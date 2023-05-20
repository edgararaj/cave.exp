#include "combat.h"
#include "player.h"
#include <ncurses.h>

void displayGameWindow(WINDOW* terminalWin, Player_Stats *player) {

    // Criar janela do mini-terminal
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
}
