#include "combat.h"
#include "player.h"
#include <ncurses.h>

void displayHUD(Warrior *warrior, Player_Stats *player)
{
    // Limpar a linha de status
    move(37, 20);
    clrtoeol();

    // Imprimir informações do jogador
    printw("HP: %d/%d  Mana: %d/%d  Level: %d  XP: %d  Gold: %d", player->hp, player->maxHP, player->mana,
           player->maxMana, player->level, player->experience, player->gold);

    // Atualizar a tela
    refresh();
}
