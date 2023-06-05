#include "combat.h"
#include "utils.h"
#include "term.h"

void warrior_attack(Warrior *a, Warrior *b, int delta_ms)
{
    if (timer_update(&a->dmg_cooldown, delta_ms))
    {
        if (vec2f_dot(rect_float_center(a->rect), rect_float_center(b->rect)) <= a->weight * a->weight)
        {
            add_term_line("HP: %d, -%d\n", b->hp, a->dmg);
            b->hp -= a->dmg;
            a->dmg_cooldown = 1000;
            if (b->hp <= 0)
            {
                a->kills += 1;
            }
<<<<<<< HEAD
            a->dmg_cooldown = 1000; // Moved inside the if statement
=======
>>>>>>> e327de03af0bf7848deab2c7f917374d7749537c
        }
        a->dmg_cooldown = 1000;
    }
}