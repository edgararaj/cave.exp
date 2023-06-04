#include "combat.h"
#include "utils.h"
#include "term.h"

int warrior_attack(Warrior *a, Warrior *b, int delta_us)
{
    if (vec2f_sqrdistance(rect_float_center(a->rect), rect_float_center(b->rect)) <= a->weight * a->weight)
    {
        b->hp -= a->dmg;
        if (b->hp <= 0)
        {
            a->kills += 1;
        }
        return 1;
    }
    return 0;
}