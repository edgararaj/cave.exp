#pragma once
#include <ncurses.h>
#include "state.h"

void init_inventory(Inventory *inventory, int capacity);
void free_inventory(Inventory *inventory);
int add_item(Inventory *inventory, Item type, int count);
int add_inventory(Inventory* inventory, Inventory other);
int remove_item(Inventory *inventory, int index);
void draw_inventory(WINDOW *win, Inventory *inventory, Vec2i window_size, int);
