#include "combat.h"
#include "term.h"
#include "utils.h"

void warrior_attack(Warrior *a, Warrior *b, int delta_ms)
{
    if (timer_update(&a->dmg_cooldown, delta_ms))
    {
        if (vec2f_sqrdistance(vec2f_sub(rect_float_center(a->rect), rect_float_center(b->rect))) <=
            a->weight * a->weight)
        {
            add_term_line("HP: %d, -%d\n", b->hp, a->dmg);
            b->hp -= a->dmg;
            if (b->hp <= 0)
            {
                a->kills += 1;
            }
            a->dmg_cooldown = 1000; // Moved inside the if statement
        }
    }
}
