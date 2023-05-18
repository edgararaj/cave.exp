#include "inventory.h"
#include <stdlib.h>

void init_inventory(Inventory *inventory, int capacity)
{
    inventory->items = malloc(capacity * sizeof(Item));
    inventory->size = 0;
    inventory->capacity = capacity;
}

void free_inventory(Inventory *inventory)
{
    free(inventory->items);
}

int add_item(Inventory *inventory, Item item)
{
    if (inventory->size < inventory->capacity)
    {
        inventory->items[inventory->size] = item;
        inventory->size++;
        return 1;
    }
    return 0;
}

int remove_item(Inventory *inventory, int index)
{
    if (index < inventory->size)
    {
        for (int i = index; i < inventory->size - 1; i++)
        {
            inventory->items[i] = inventory->items[i + 1];
        }
        inventory->size--;
        return 1;
    }
    return 0;
}

void draw_inventory(WINDOW *win, Inventory *inventory)
{
    werase(win);
    wprintw(win, "Inventory:\n");
    for (int i = 0; i < inventory->size; i++)
    {
        wprintw(win, "%d. %c - %s\n", i + 1, inventory->items[i].symbol, inventory->items[i].name);
    }
    wrefresh(win);
}
