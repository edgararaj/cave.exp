#include "inventory.h"
#include "items.h"
#include <stdlib.h>

void init_inventory(Inventory *inventory, int capacity)
{
    inventory->items = malloc(capacity * sizeof(Item));
    inventory->item_counts = malloc(capacity * sizeof(int)); // Initialize item_counts array
    for (int i = 0; i < capacity; i++)
    {
        inventory->item_counts[i] = 0; // Initialize all counts to 0
    }
    inventory->size = 0;
    inventory->capacity = capacity;
}

void free_inventory(Inventory *inventory)
{
    free(inventory->items);
    free(inventory->item_counts);
}

int add_item(Inventory *inventory, Item item)
{
    for (int i = 0; i < inventory->size; i++)
    {
        if (inventory->items[i].type == item.type) // If the item is already in the inventory
        {
            inventory->item_counts[i]++; // Increase the count of the item
            return 1;
        }
    }
    if (inventory->size < inventory->capacity) // If the item is not in the inventory and there's space
    {
        inventory->items[inventory->size] = item;
        inventory->item_counts[inventory->size] = 1; // Set the count of the new item to 1
        inventory->size++;
        return 1;
    }
    return 0;
}

int remove_item(Inventory *inventory, int index)
{
    if (index < inventory->size)
    {
        inventory->item_counts[index]--;        // Decrease the count of the item
        if (inventory->item_counts[index] == 0) // If the count of the item becomes 0
        {
            for (int i = index; i < inventory->size - 1; i++)
            {
                inventory->items[i] = inventory->items[i + 1];
                inventory->item_counts[i] = inventory->item_counts[i + 1]; // Shift the counts of the items
            }
            inventory->size--;
        }
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
        wprintw(win, "%d. %c - %s x%d\n", i + 1, inventory->items[i].symbol, inventory->items[i].name,
                inventory->item_counts[i]);
    }
    wrefresh(win);
}
