#pragma once
#include "items.h"

#include <ncurses.h>

typedef struct Inventory
{
    Item *items;
    int size;
    int capacity;
    int gold;
} Inventory;

void init_inventory(Inventory *inventory, int capacity);
void free_inventory(Inventory *inventory);
int add_item(Inventory *inventory, Item item);
int remove_item(Inventory *inventory, int index);
void draw_inventory(WINDOW *win, Inventory *inventory);
