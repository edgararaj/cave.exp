#include "dist.h"
#include "map.h"
#include "objects.h"
#include <assert.h>

void shine_reset(Bitmap distmap)
{
    for (int x = 0; x < distmap.width; x++)
    {
        for (int y = 0; y < distmap.height; y++)
        {
        }
    }
}

void dist_pass_recursive(Bitmap distmap, Vec2i point, int value, Bitmap illuminated)
{
    if (value >= MAX_DIST_CALC)
        return;
    if (map_is_wall(distmap, vec2i_to_f(point)) && value <= MAX_DIST_SHINE)
    {
        set_normal_map_value(distmap, point, SHINE);
        illuminated.data[point.y * illuminated.width + point.x] = SHINE;
        return;
    }
    int data = get_dist_map_value(distmap, point);
    if (data <= value && data >= 1)
        return;

    set_dist_map_value(distmap, point, value);
    illuminated.data[point.y * illuminated.width + point.x] = WALKABLE;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            Vec2i add = {i, j};
            Vec2f pos = vec2i_to_f(point);
            Vec2f new_pos = vec2i_to_f(vec2i_add(point, add));
            if (map_is_wall(distmap, new_pos) || (!map_is_wall(distmap, new_pos) && map_is_walkable(distmap, pos, vec2i_to_f(add))))
                dist_pass_recursive(distmap, vec2i_add(point, add), value + 1, illuminated);
        }
    }
}

void dist_pass(Bitmap distmap, Vec2i point, Bitmap illuminated)
{
    dist_pass_recursive(distmap, point, 1, illuminated);
}

void dist_reset(Bitmap distmap)
{
    for (int x = 0; x < distmap.width; x++)
    {
        for (int y = 0; y < distmap.height; y++)
        {
            set_dist_map_value(distmap, (Vec2i){x, y}, 0);
        }
    }
}