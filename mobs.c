#include "mobs.h"
#include "combat.h"
#include "map.h"
#include "objects.h"
#include "state.h"
#include "utils.h"

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
        mobs[i].warrior.maxHP = 100;
        mobs[i].warrior.weight = 3;
        mobs[i].warrior.dmg_cooldown = 1000;
    }
}

Vec2i step_to_player(Bitmap map, Mob* mob)
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
    return smallest_add;
}

void update_mob(Mob* mobs, int num_mobs, int ii, Bitmap map, Warrior *player, int delta_ms)
{
    Mob* mob = &mobs[ii];
    if (mob->type == MobType_Stupid)
    {
        if (vec2f_sqrdistance(rect_float_center(mob->warrior.rect), rect_float_center(player->rect)) < THREAT_RADIUS_SQR)
        {   
            // add_term_line("Stupid mob is attacking you!\n");
            // Feeling threatened
            mob->warrior.rect.color = COLOR_RED;

            Vec2i step = step_to_player(map, mob);
            mob->warrior.rect = rect_float_translate(mob->warrior.rect, vec2f_div_const(vec2i_to_f(step), mob->speed * 10));
            warrior_attack(&mob->warrior, player, delta_ms);
        }
        else {
            mob->warrior.rect.color = COLOR_BLUE;
        }
    }
    else if (mob->type == MobType_Coward || mob->type == MobType_Intelligent)
    {
        int mob_dist_to_player = vec2f_sqrdistance(rect_float_center(mob->warrior.rect), rect_float_center(player->rect));
        // if (dist_to_player < THREAT_RADIUS_SQR)
        // {
        //     // Feeling threatened
        //     mob->warrior.rect.color = COLOR_RED;

        //     Vec2i step = step_to_player(map, mob);
        //     mob->warrior.rect = rect_float_translate(mob->warrior.rect, vec2f_div_const(vec2i_to_f(step), mob->speed * 10));
        //     warrior_attack(&mob->warrior, player, delta_ms);
        // }

        if (mob_dist_to_player < VISION_RADIUS_SQR)
        {
            mob->warrior.rect.color = COLOR_YELLOW;
            int mobs_near = 0;
            for (int i = 0; i < num_mobs; i++)
            {
                int mob_dist_to_caller = vec2f_sqrdistance(rect_float_center(mobs[i].warrior.rect), rect_float_center(mob->warrior.rect));
                if ((ii != i) && (mob_dist_to_caller <= THREAT_RADIUS_SQR))
                {
                    mobs_near++;
                }
            }
            for (int i = 0; i < num_mobs; i++)
            {
                mob_dist_to_player = vec2f_sqrdistance(rect_float_center(mobs[i].warrior.rect), rect_float_center(player->rect));
                int mob_dist_to_caller = vec2f_sqrdistance(rect_float_center(mobs[i].warrior.rect), rect_float_center(mob->warrior.rect));
                if (((i != ii) && mob_dist_to_player >= THREAT_RADIUS_SQR) || (ii == i && mobs_near))
                {
                    // Is Calling other
                    mob->warrior.rect.color = COLOR_GREEN;
                    Vec2i step = step_to_player(map, &mobs[i]);
                    mobs[i].warrior.rect = rect_float_translate(mobs[i].warrior.rect, vec2f_div_const(vec2i_to_f(step), mobs[i].speed * 10));
                    warrior_attack(&mobs[i].warrior, player, delta_ms);
                }
            }
        }
        else {
            mob->warrior.rect.color = COLOR_BLUE;
        }
    }
    // else if (mob->type == MobType_Intelligent)
    // {
    //     // if (vec2f_sqrdistance(rect_float_center(mob->warrior.rect), rect_float_center(player->rect)) < THREAT_RADIUS_SQR)
    //     // {
    //     //     // add_term_line("Intelligent mob is attacking you!\n");
    //     // }
    // }
}

void update_mobs(Mob *mobs, int num_mobs, Bitmap map, Warrior *player, int delta_ms)
{
    for (int i = 0; i < num_mobs; i++)
    {
        if (mobs[i].warrior.hp <= 0)
            continue;
        update_mob(mobs, num_mobs, i, map, player, delta_ms);
    }
}