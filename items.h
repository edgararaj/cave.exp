#pragma once
#include "player.h"

// Define the maximum number of items a player can carry
#define MAX_INVENTORY_SIZE 10

// Define the item type
typedef enum
{
    ITEM_TYPE_SWORD,
    ITEM_TYPE_BLASTGUN,
    ITEM_TYPE_HEALTH_POTION,
    ITEM_TYPE_MANA_POTION,
    ITEM_TYPE_COINS,
    ITEM_TYPE_KEY,
} ItemType;


// Define the item structure
typedef struct
{
    ItemType type;
    char *name;
    char symbol;
    int color;
} Item;


