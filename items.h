#pragma once
#include "player.h"

// Define the maximum number of items a player can carry
#define MAX_INVENTORY_SIZE 10

// Define the item type
typedef enum
{
    ITEM_TYPE_HEALTH_POTION,
    ITEM_TYPE_MANA_POTION,
    // Add more types here as you develop your game
} ItemType;

// Define the item structure
typedef struct
{
    ItemType type;
    char *name;
    char symbol;
    int color;
} Item;
