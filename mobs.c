#include "game.h"
#include "mobs.h"
#include "collide.h"
#include "combat.h"
#include "dist.h"
#include "light.h"
#include "map.h"
#include "objects.h"
#include "state.h"
#include "utils.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "colors.h"

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
        } while (get_bitmap_value(pixmap, (Vec2i){x, y}) != WALKABLE);

        mobs[i] = (Mob){0};
        mobs[i].type = random_between(0, MobType__Size);
        mobs[i].warrior.rect = (RectFloat){{x, y}, {x + width - 1, y + height - 1}, 6};
        mobs[i].warrior.dmg = random_between(2, 20);
        mobs[i].speed = random_between(1, 10);
        mobs[i].warrior.hp = random_between(10, 100);
        mobs[i].warrior.maxHP = 100;
        mobs[i].warrior.weight = 3;
    }
}

Vec2i step_to_player(Bitmap map, Mob *mob)
{
    int smallest = MAX_DIST_CALC;
    Vec2i smallest_add = {0, 0};
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (i + j == -1 || i + j == 1)
            {
                Vec2i add = {i, j};
                Vec2f rect = vec2f_add(rect_float_center(mob->warrior.rect), vec2i_to_f(add));
                int data = get_bitmap_value(map, vec2f_to_i(rect));
                if (data < smallest && data && data <= MAX_DIST_CALC)
                {
                    smallest = data;
                    smallest_add = add;
                }
            }
        }
    }
    return smallest_add;
}

void move_mob(Mob *mob, Vec2i step, int delta_us)
{
    mob->warrior.rect = rect_float_translate(mob->warrior.rect, vec2f_mul_const(vec2i_to_f(step), mob->speed * delta_us / 1e6));
}

void attack_player(Mob *mob, Warrior *player, Bitmap map, int delta_us)
{
    if (vec2f_sqrdistance(rect_float_center(mob->warrior.rect), rect_float_center(player->rect)) >
        mob->warrior.weight * mob->warrior.weight)
    {
        Vec2i step = step_to_player(map, mob);
        move_mob(mob, step, delta_us);
    }

    if (timer_update(&mob->warrior.dmg_cooldown, delta_us))
    {
        if (warrior_attack(&mob->warrior, player)) {
            mob->warrior.dmg_cooldown = 1e6;
            mob->warrior.attacking = 0.1 * 1e6;
        }
    }
}

Arrow attack_player_with_arrow(Mob *mob, Warrior *player)
{
    Arrow result;
    Vec2f vec = vec2f_sub(rect_float_center(player->rect), rect_float_center(mob->warrior.rect));
    vec = vec2f_normalize(vec);
    result.velocity = vec;
    result.rect = mob->warrior.rect;

    return result;
}

void wander(Mob *mob, Bitmap map, int delta_us)
{
    if (mob->wander_to.x != 0 && mob->wander_to.y != 0)
    {
        RectFloat new_rect =
            rect_float_translate(mob->warrior.rect, vec2f_mul_const(vec2i_to_f(mob->wander_to), mob->speed * delta_us / 1e6));
        if (!collide_rect_bitmap(rect_float_to_rect(new_rect), map) && rand() % 100 < 10)
        {
            mob->warrior.rect = new_rect;
            return;
        }
    }
    int r = random_between(0, LIGHT_RADIUS);
    int a = random_between(0, 2 * M_PI);
    Vec2i step = {r * cos(a), r * sin(a)};
    mob->wander_to = step;
    // for (int i = -1; i < 2; i++)
    // {
    //     for (int j = -1; j < 2; j++)
    //     {
    //         Vec2i add = {i, j};
    //         if (!map_is_wall(map, vec2f_add(rect_float_center(mob->warrior.rect), vec2i_to_f(add))) &&
    //             (rand() % 100 < 20))
    //         {
    //             move_mob(mob, add, delta_us);
    //         }
    //     }
    // }
}

void call_others(Mob *mobs, int num_mobs)
{
    for (int i = 0; i < num_mobs; i++)
    {
        mobs[i].called = 1;
    }
}

void update_mob(Mob *mobs, int num_mobs, int ii, Bitmap map, Warrior *player, Bitmap player_light, int delta_us, Arrow* arrows, int *arrow_count)
{
    Mob *mob = &mobs[ii];
    int mob_dist_to_player = LIGHT_RADIUS - get_bitmap_value(player_light, vec2f_to_i(rect_float_center(mob->warrior.rect)));
    if (mob->type == MobType_Stupid)
    {
        if (mob_dist_to_player < THREAT_RADIUS || mob->called)
        {
            if (mob->called)
                mob->warrior.rect.color = Color_Blue;
            else
                mob->warrior.rect.color = Color_Red;
            attack_player(mob, player, map, delta_us);
        }
        else if (mob_dist_to_player < LIGHT_RADIUS)
        {
            mob->warrior.rect.color = Color_Yellow;
        }
        else
        {
            mob->warrior.rect.color = Color_White;
            wander(mob, map, delta_us);
        }
    }
    else if (mob->type == MobType_Coward || mob->type == MobType_Intelligent)
    {
        if (mob_dist_to_player < THREAT_RADIUS || mob->called)
        {
            call_others(mobs, num_mobs);
            if (mob->called)
            {
                mob->warrior.rect.color = Color_Blue;
            }
            else
            {
                mob->warrior.rect.color = Color_Red;
            }
            attack_player(mob, player, map, delta_us);
        }
        else if (mob_dist_to_player < LIGHT_RADIUS)
        {
            call_others(mobs, num_mobs);
            int mobs_near = 0;
            for (int i = 0; i < num_mobs; i++)
            {
                int mob_dist_to_caller =
                    vec2f_sqrdistance(rect_float_center(mobs[i].warrior.rect), rect_float_center(mob->warrior.rect));
                if ((ii != i) && (mob_dist_to_caller <= THREAT_RADIUS_SQR))
                {
                    mobs_near++;
                }
            }
            if (mobs_near)
            {
                mob->warrior.rect.color = Color_Red;
                attack_player(mob, player, map, delta_us);
            }
            else
            {
                mob->warrior.rect.color = Color_Yellow;
            }
        }
        else
        {
            mob->warrior.rect.color = Color_White;
            wander(mob, map, delta_us);
        }
    }
    else if (mob->type == MobType_Archer)
    {
        if (mob_dist_to_player < THREAT_RADIUS || mob->called)
        {
            // call_others(mobs, num_mobs, ii, player_light);
            if (mob->called)
            {
                mob->warrior.rect.color = Color_Blue;
            }
            else
            {
                mob->warrior.rect.color = Color_Red;
            }
            attack_player(mob, player, map, delta_us);
        }
        else if (mob_dist_to_player < LIGHT_RADIUS)
        {
            // call_others(mobs, num_mobs, ii, player_light);
            // int mobs_near = 0;
            // for (int i = 0; i < num_mobs; i++)
            // {
            //     int mob_dist_to_caller =
            //         vec2f_sqrdistance(rect_float_center(mobs[i].warrior.rect), rect_float_center(mob->warrior.rect));
            //     if ((ii != i) && (mob_dist_to_caller <= THREAT_RADIUS_SQR))
            //     {
            //         mobs_near++;
            //     }
            // }
            // if (mobs_near)
            // {
            mob->warrior.rect.color = Color_Red;
            if (*arrow_count < MAX_ARROWS && timer_update(&mob->warrior.dmg_cooldown, delta_us))
            {
                mob->warrior.dmg_cooldown = 1e6;
                arrows[(*arrow_count)] = attack_player_with_arrow(mob, player);
                *arrow_count += 1;
            }
            // }
            // else
            // {
            //     mob->warrior.rect.color = Color_Yellow;
            // }
        }
        else
        {
            mob->warrior.rect.color = COLOR_WHITE;
            wander(mob, map, delta_us);
        }
    }
}

void update_mobs(Mob *mobs, int num_mobs, Bitmap map, Warrior *player, Bitmap player_light, int delta_us, Arrow* arrows, int* arrow_count)
{
    for (int i = 0; i < num_mobs; i++)
    {
        if (mobs[i].warrior.hp <= 0)
            continue;
        mobs[i].called = 0;
    }
    for (int i = 0; i < num_mobs; i++)
    {
        if (mobs[i].warrior.hp <= 0)
            continue;
        update_mob(mobs, num_mobs, i, map, player, player_light, delta_us, arrows, arrow_count);
    }
}
