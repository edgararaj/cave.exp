#include <ncurses.h>

#include "combat.h"
#include "items.h"
#include "player.h"
#include "term.h"

// // Function to add an item to the player's inventory
// void add_item_to_inventory(Inventory *inventory, Item item)
// {
//     if (inventory->size < MAX_INVENTORY_SIZE)
//     {
//         inventory->items[inventory->size] = item;
//         player->inventorySize++;
//         add_term_line("Added %s to inventory.\n", item.name);
//     }
//     else
//     {
//         add_term_line("Inventory is full, cannot add %s.\n", item.name);
//     }
// }

// // Function to remove an item from the player's inventory
// void remove_item_from_inventory(Inventory *player, int index)
// {
//     if (index < 0 || index >= player->inventorySize)
//     {
//         add_term_line("Invalid inventory index: %d.\n", index);
//         return;
//     }
//     // Shift all items after the removed item to fill the gap
//     for (int i = index; i < player->inventorySize - 1; i++)
//     {
//         player->inventory[i] = player->inventory[i + 1];
//     }
//     player->inventorySize--;
//     add_term_line("Removed item from inventory.\n");
// }

// Function to use a health potion
void use_health_potion(Warrior *player)
{
    if (player->hp < player->maxHP)
    {
        player->hp += 50; // Aumenta 50 HP
        if (player->hp > player->maxHP)
        {
            player->hp = player->maxHP;
        }
        add_term_line("Health Potion used! HP is now %d.", player->hp);
    }
    else
    {
        add_term_line("HP is full, cannot use Health Potion.");
    }
}

// // Function to use a mana potion
// void use_mana_potion(Player_Stats *player, Item *item)
// {
//     if (player->mana < player->maxMana)
//     {
//         player->mana += 50;
//         if (player->mana > player->maxMana)
//         {
//             player->mana = player->maxMana;
//         }
//         // add_term_line("%s used! Mana is now %d.\n", item->name, player->mana);
//     }
//     else
//     {
//         // add_term_line("Mana is full, cannot use %s.\n", item->name);
//     }
// }
