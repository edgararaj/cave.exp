#include "objects.h"
#include "map.h"

void dist_pass_recursive(Bitmap distmap, Vec2i point, int value)
{
    if (value >= MAX_DIST) return;
    int data = distmap.data[point.y * distmap.width + point.x];
    if (map_is_wall(distmap, vec2i_to_f(point))) {
        // HACK: Put shine on walls
        distmap.data[point.y * distmap.width + point.x] = SHINE;
        return;
    };
    if (data <= value && data >= DIST_BASE) return;
    distmap.data[point.y * distmap.width + point.x] = value;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            Vec2i add = {i, j};
            dist_pass_recursive(distmap, vec2i_add(point, add), value + 1);
        }
    }
}

void dist_pass(Bitmap distmap, Vec2i point)
{
    dist_pass_recursive(distmap, point, DIST_BASE);
}

void dist_reset(Bitmap distmap)
{
    for (int x = 0; x < distmap.width; x++)
    {
        for (int y = 0; y < distmap.height; y++)
        {
            if (distmap.data[y * distmap.width + x] >= DIST_BASE && distmap.data[y * distmap.width + x] < MAX_DIST)
            {
                distmap.data[y * distmap.width + x] = WALKABLE;
            }
            if (distmap.data[y * distmap.width + x] == SHINE)
            {
                distmap.data[y * distmap.width + x] = WALL;
            }
        }
    }
}