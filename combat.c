#include "combat.h"
#include "utils.h"
#include "term.h"

void warrior_attack(Warrior *a, Warrior *b, int delta_ms)
{
    if (timer_update(&a->dmg_cooldown, delta_ms))
    {
        if (vec2f_dot(rect_float_center(a->rect), rect_float_center(b->rect)) <= a->weight * a->weight)
        {
            b->hp -= a->dmg;
            a->dmg_cooldown = 1000;
            if (b->hp <= 0)
            {
                a->kills += 1;
            }
            a->dmg_cooldown = 1000; // Moved inside the if statement
        }
        a->dmg_cooldown = 1000;
    }
}