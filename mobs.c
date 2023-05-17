#include "mobs.h"
#include "combat.h"
#include "map.h"
#include "objects.h"
#include "state.h"

void create_mobs(Bitmap pixmap, Mob *mobs, int num_mobs)
{
    int width = 1;
    int height = 1;
    for (int i = 0; i < num_mobs; i++)
    {
        int x, y;
        do
        {
            x = random_between(0, pixmap.width - width);
            y = random_between(0, pixmap.height - height);
        } while (normal_map_decode(pixmap.data[y * pixmap.width + x]) != WALKABLE);

        mobs[i].type = random_between(0, MobType__Size);
        mobs[i].warrior.rect = (RectFloat){{x, y}, {x + width - 1, y + height - 1}, 6};
        mobs[i].warrior.dmg = random_between(2, 20);
        mobs[i].speed = random_between(1, 5);
        mobs[i].warrior.hp = random_between(10, 100);
    }
}

void update_mob(Mob *mob, Bitmap map, Warrior *player)
{
    int smallest = MAX_DIST;
    Vec2i smallest_add = {0, 0};
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (i + j == -1 || i + j == 1)
            {
                Vec2i add = {i, j};
                RectFloat rect = rect_float_translate(mob->warrior.rect, vec2i_to_f(add));
                int data = dist_map_decode(map.data[(int)(rect.tl.y) * map.width + (int)rect.tl.x]);
                if (data < smallest && data && data <= MAX_DIST)
                {
                    smallest = data;
                    smallest_add = add;
                }
            }
        }
    }
    mob->warrior.rect =
        rect_float_translate(mob->warrior.rect, vec2f_div_const(vec2i_to_f(smallest_add), mob->speed * 10));
    warrior_attack(&mob->warrior, player);
}

void update_mobs(Mob *mobs, int num_mobs, Bitmap map, Warrior *player)
{
    for (int i = 0; i < num_mobs; i++)
    {
        if (mobs[i].warrior.hp <= 0)
            continue;
        update_mob(&mobs[i], map, player);
    }
}