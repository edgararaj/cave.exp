#pragma once
#include "items.h"
#include <ncurses.h>

#define HOTBAR_SIZE 9

typedef struct Inventory
{
    Item *items;
    int *item_counts; // Array to store the count of each item
    int size;
    int capacity;
    int gold;
} Inventory;

typedef struct {
    int type; 
    int count; 
} HotbarItem;


typedef struct {
    HotbarItem items[HOTBAR_SIZE]; // The items in the hotbar
    int selected; // The currently selected item
} Hotbar;



void init_inventory(Inventory *inventory, int capacity);
void free_inventory(Inventory *inventory);
int add_item(Inventory *inventory, Item item);
int remove_item(Inventory *inventory, int index);
void draw_inventory(WINDOW *win, Inventory *inventory);
