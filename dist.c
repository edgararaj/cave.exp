#include "dist.h"
#include "map.h"
#include "objects.h"
#include <assert.h>

void dist_pass_recursive(Bitmap distmap, Bitmap collision, Vec2i point, int value, Bitmap illuminated)
{
    if (value >= MAX_DIST_CALC)
        return;
    if (map_is_wall(collision, vec2i_to_f(point)))
    {
        if (value < MAX_DIST_SHINE)
        {
            set_bitmap_value(collision, point, SHINE);
            set_bitmap_value(illuminated, point, SHINE);
        }
        return;
    };
    int distance_data = get_bitmap_value(distmap, point);
    if (distance_data <= value && distance_data >= 1)
        return;

    set_bitmap_value(distmap, point, value);
    set_bitmap_value(illuminated, point, WALKABLE);
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            Vec2i add = {i, j};
            if (map_is_wall(collision, vec2i_to_f(vec2i_add(point, add))) || (!map_is_wall(collision, vec2i_to_f(vec2i_add(point, add))) && map_is_walkable(collision, vec2i_to_f(point), vec2i_to_f(add))))
                dist_pass_recursive(distmap, collision, vec2i_add(point, add), value + 1, illuminated);
        }
    }
}

void dist_pass(Bitmap distmap, Bitmap collision, Vec2i point, Bitmap illuminated)
{
    dist_pass_recursive(distmap, collision, point, 1, illuminated);
}
