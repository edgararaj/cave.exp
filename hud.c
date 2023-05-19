#include "combat.h"
#include "player.h"
#include <ncurses.h>

void displayHUD(Player_Stats *player)
{
    // Clear the status line
    move(37, 20);
    clrtoeol();

    // Print player information
    printw("HP: %d/%d  Mana: %d/%d  Level: %d  XP: %d  Gold: %d", player->hp, player->maxHP, player->mana,
           player->maxMana, player->level, player->experience, player->gold);

    // Update the screen
    refresh();
}
