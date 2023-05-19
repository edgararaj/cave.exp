#include "combat.h"
#include "player.h"
#include <ncurses.h>

void displayHUD(GameState *gs, Player_Stats *player)
{
    // Limpar a linha de status
    move(0, 20);
    clrtoeol();

    // Print player information
    // printw("HP: %d/%d  Mana: %d/%d  Level: %d  XP: %d  Gold: %d", player->hp, player->maxHP, player->mana,
    //   player->maxMana, player->level, player->experience, player->gold);

    werase(gs->vida);
    wprintw(gs->vida, "Inventory:\n");
   // for (int i = 0; i < 4; i++)
   // {
   //     wprintw(win_vida, "%d. %c - %s x%d\n", i + 1, inventory->items[i].symbol, inventory->items[i].name,
   //             inventory->item_counts[i]);
   // }
   // wrefresh(win);

int i = 0;
    wprintw(gs->vida, "HP: %d/%d\n ", i + 1,  player->hp, player->maxHP);
    move(1, 20);
    printw("Mana: %d/%d\n", player->mana, player->maxMana);
    move(2, 20);
    printw("  Level: %d\n", player->level);
    move(3, 20);
    printw("XP: %d\n", player->experience);
    move(4, 20);
    printw("Gold: %d\n", player->gold);

    // Update the screen
    refresh();
}
