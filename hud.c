#include "player.h" // A presumir que a struct do Player esteja definida neste ficheiro
#include <ncurses.h>

void displayHUD(Player_Stats *player)
{
    // Limpar a linha de status
    move(0, 0);
    clrtoeol();

    // Imprimir informações do jogador
    printw("Lives: %d/%d  Mana: %d/%d  Level: %d  XP: %d  Gold: %d", player->lives, player->maxLives, player->mana,
           player->maxMana, player->level, player->experience, player->gold);

    // Atualizar a tela
    refresh();
}
