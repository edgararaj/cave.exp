#include "items.h"
#include "player.h"

#include <ncurses.h>

// Function to add an item to the player's inventory
void add_item_to_inventory(Player *player, Item item)
{
    if (player->inventorySize < MAX_INVENTORY_SIZE)
    {
        player->inventory[player->inventorySize] = item;
        player->inventorySize++;
        printf("Added %s to inventory.\n", item.name);
    }
    else
    {
        printf("Inventory is full, cannot add %s.\n", item.name);
    }
}

// Function to remove an item from the player's inventory
void remove_item_from_inventory(Player *player, int index)
{
    if (index < 0 || index >= player->inventorySize)
    {
        printf("Invalid inventory index: %d.\n", index);
        return;
    }
    // Shift all items after the removed item to fill the gap
    for (int i = index; i < player->inventorySize - 1; i++)
    {
        player->inventory[i] = player->inventory[i + 1];
    }
    player->inventorySize--;
    printf("Removed item from inventory.\n");
}

// Function to use a health potion
void use_health_potion(Player_Stats *player, Item *item)
{
    if (player->lives < player->maxLives)
    {
        player->lives += 50; // Restore 50 HP
        if (player->lives > player->maxLives)
        {
            player->lives = player->maxLives; // Cap the HP at maxLives
        }
        printf("%s used! HP is now %d.\n", item->name, player->lives);
    }
    else
    {
        printf("HP is full, cannot use %s.\n", item->name);
    }
}

// Function to use a mana potion
void use_mana_potion(Player_Stats *player, Item *item)
{
    if (player->mana < player->maxMana)
    {
        player->mana += 50; // Restore 50 mana
        if (player->mana > player->maxMana)
        {
            player->mana = player->maxMana; // Cap the mana at maxMana
        }
        printf("%s used! Mana is now %d.\n", item->name, player->mana);
    }
    else
    {
        printf("Mana is full, cannot use %s.\n", item->name);
    }
}

// Function to use an item
void use_item(Player_Stats *player, Item *item)
{
    switch (item->type)
    {
    case ITEM_TYPE_HEALTH_POTION:
        use_health_potion(player, item);
        break;
    case ITEM_TYPE_MANA_POTION:
        use_mana_potion(player, item);
        break;
    default:
        printf("Unknown item type!\n");
        break;
    }
}
