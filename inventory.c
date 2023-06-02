#include <stdlib.h>
#include <string.h>

#include "inventory.h"
#include "items.h"
#include "term.h"
#include "colors.h"

int add_item(Inventory *inventory, Item name, int count)
{
    for (int i = 0; i < inventory->size; i++)
    {
        if (inventory->items[i].name == name) // If the item is already in the inventory
        {
            inventory->items[i].count += count; // Increase the count of the item
            return 1;
        }
    }
    if (inventory->size < MAX_INVENTORY_SIZE) // If the item is not in the inventory and there's space
    {
        inventory->items[inventory->size].name = name;
        inventory->items[inventory->size].count = count; // Set the count of the new item to 1
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
        if (other.items[i].name == Item_Key)
        {
            add_term_line("You found a key, find the portal!\n");
        }
        add_item(inventory, other.items[i].name, other.items[i].count);
    }
    return 1;
}

int remove_item_name(Inventory *inventory, Item name)
{
    for (int i = 0; i < inventory->size; i++)
    {
        if (inventory->items[i].name == name) // If the item is already in the inventory
        {
            inventory->items[i].count--; // Decrease the count of the item
            return 1;
        }
    }
    return 0;
}

int remove_item(Inventory *inventory, int index)
{
    if (index < inventory->size)
    {
        inventory->items[index].count--;        // Decrease the count of the item
        // if (inventory->items_counts[index] == 0) // If the count of the item becomes 0
        // {
        //     for (int i = index; i < inventory->size - 1; i++)
        //     {
        //         inventory->items[i] = inventory->items[i + 1];
        //         inventory->items_counts[i] = inventory->items_counts[i + 1]; // Shift the counts of the items
        //     }
        // } //
        //
        return 1;
    }
    return 0;
}

void draw_inventory(WINDOW *win, Inventory *inventory, Vec2i window_size, int selected)
{
    for (int i = 0; i < inventory->size; i++)
    {
        char* name = "Unknown";
        if (inventory->items[i].name == Item_HealthPotion)
            name = "Health Potion";
        else if (inventory->items[i].name == Item_ManaPotion)
            name = "Mana Potion";
        else if (inventory->items[i].name == Item_Key)
            name = "Key";
        else if (inventory->items[i].name == Item_Sword)
            name = "Sword";
        else if (inventory->items[i].name == Item_BlastGun)
            name = "Blast Gun";

        if (inventory->items[i].count > 0)
        {
            if (i + 1 == selected)
                wattrset(win, COLOR_PAIR(Culur_Default_Green));
            else
                wattrset(win, COLOR_PAIR(Culur_Default));

            if (inventory->items[i].type != ItemType_Disposable)
                mvwprintw(win, i+1, 1, "%d. %s\n", i + 1, name);
            else
                mvwprintw(win, i+1, 1, "%d. %s x%d\n", i + 1, name, inventory->items[i].count);
        }
        else if (inventory->items[i].count == 0)
        {
            if (i + 1 == selected)
            {
                wattrset(win, COLOR_PAIR(Culur_Default_Red));
                mvwprintw(win, i+1, 1, "%d. %s (Empty)\n", i + 1, name);
            }
            else
            {
                wattrset(win, COLOR_PAIR(Culur_Default_Gray));
                mvwprintw(win, i+1, 1, "%d. %s\n", i + 1, name);
            }
        }
    }
    wattrset(win, COLOR_PAIR(Culur_Default));
    box(win, 0, 0);
    char title[] = "INVENTORY";
    mvwprintw(win, 0, (window_size.x - strlen(title)) / 2, "%s", title);
}
