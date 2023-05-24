#include "inventory.h"
#include "items.h"
#include <stdlib.h>
#include "term.h"
#include <string.h>

int add_item(Inventory *inventory, ItemType type, int count)
{
    for (int i = 0; i < inventory->size; i++)
    {
        if (inventory->items[i] == type) // If the item is already in the inventory
        {
            inventory->items_counts[i]+= count; // Increase the count of the item
            return 1;
        }
    }
    if (inventory->size < MAX_INVENTORY_SIZE) // If the item is not in the inventory and there's space
    {
        inventory->items[inventory->size] = type;
        inventory->items_counts[inventory->size] = count; // Set the count of the new item to 1
        inventory->size++;
        return 1;
    }
    else {
        add_term_line("Inventory is full!\n");
    }
    return 0;
}

int add_inventory(Inventory* inventory, Inventory other)
{
    for (int i = 0; i < other.size; i++)
    {
        add_item(inventory, other.items[i], other.items_counts[i]);
    }
    return 1;
}

int remove_item(Inventory *inventory, int index)
{
    if (index < inventory->size)
    {
        inventory->items_counts[index]--;        // Decrease the count of the item
        if (inventory->items_counts[index] == 0) // If the count of the item becomes 0
        {
            for (int i = index; i < inventory->size - 1; i++)
            {
                inventory->items[i] = inventory->items[i + 1];
                inventory->items_counts[i] = inventory->items_counts[i + 1]; // Shift the counts of the items
            }
            inventory->size--;
        }
        return 1;
    }
    return 0;
}

void draw_inventory(WINDOW *win, Inventory *inventory, Vec2i window_size)
{
    for (int i = 0; i < inventory->size; i++)
    {
        char* name = "Unknown";
        if (inventory->items[i] == ItemType_HealthPotion)
            name = "Health Potion";
        mvwprintw(win, i+1, 1, "%d. %s x%d\n", i + 1, name, inventory->items_counts[i]);
    }
    box(win, 0, 0);
    char title[] = "INVENTORY";
    mvwprintw(win, 0, (window_size.x - strlen(title)) / 2, title);
}
