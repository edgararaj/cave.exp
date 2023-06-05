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
        if (inventory->items[i].name == name) // Se o item já está no inventário
        {
            inventory->items[i].count += count; // Aumenta a quantidade do item
            return 1;
        }
    }
    if (inventory->size < MAX_INVENTORY_SIZE) // Se o item não está no inventário e há espaço
    {
        inventory->items[inventory->size].name = name;
        inventory->items[inventory->size].count = count; // Mete a contagem do novo item a 1
        inventory->size++;
        return 1;
    }
    else {
        add_term_line("Inventory is full!");
    }
    return 0;
}

int add_inventory(Inventory* inventory, Inventory other)
{
    for (int i = 0; i < other.size; i++)
    {
        if (other.items[i].name == Item_Key)
        {
            add_term_line("You found a key, find the portal!");
        }
        add_item(inventory, other.items[i].name, other.items[i].count);
    }
    return 1;
}

int remove_item_name(Inventory *inventory, Item name)
{
    for (int i = 0; i < inventory->size; i++)
    {
        if (inventory->items[i].name == name) // Se o item já está no inventário
        {
            inventory->items[i].count--; // Diminui a contagem do item
            return 1;
        }
    }
    return 0;
}

int remove_item(Inventory *inventory, int index)
{
    if (index < inventory->size)
    {
        inventory->items[index].count--;        // Diminui a contagem do item
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

void split_string(char* str, char* buffer, int n)
{
    int i;
    int str_length = (int) strlen(str);
    for (i = 0; i < str_length - n; i++)
    {
        buffer[i] = str[i + n];
    }
    buffer[i] = '\0';
    str[n] = '\0';
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
            char buffer[100];
            if (inventory->items[i].type != ItemType_Disposable)
                snprintf(buffer, 100, "%d. %s", i + 1, name);
            else
                snprintf(buffer, 100, "%d. %s x%d", i + 1, name, inventory->items[i].count);

            char buffer2[100];
            int n = (1-inventory->items[i].cooldown) * strlen(buffer);
            split_string(buffer, buffer2, n);

            if (i + 1 == selected)
                wattrset(win, COLOR_PAIR(Culur_Default_Green));
            else
                wattrset(win, COLOR_PAIR(Culur_Default));
            mvwprintw(win, i+1, 1, "%s", buffer);
            
            wattrset(win, COLOR_PAIR(Culur_Default_Blue));
            mvwprintw(win, i+1, 1+n, "%s", buffer2);
        }
        else if (inventory->items[i].count == 0)
        {
            if (i + 1 == selected)
            {
                wattrset(win, COLOR_PAIR(Culur_Default_Red));
                mvwprintw(win, i+1, 1, "%d. %s (Empty)", i + 1, name);
            }
            else
            {
                wattrset(win, COLOR_PAIR(Culur_Default_Gray));
                mvwprintw(win, i+1, 1, "%d. %s", i + 1, name);
            }
        }
    }
    wattrset(win, COLOR_PAIR(Culur_Default));
    box(win, 0, 0);
    char title[] = "INVENTORY";
    mvwprintw(win, 0, (window_size.x - strlen(title)) / 2, "%s", title);
}
