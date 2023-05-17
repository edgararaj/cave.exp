#include "map.h"
#include "objects.h"

void dist_pass_recursive(Bitmap distmap, Vec2i point, int value,
                         Bitmap illuminated) {
    if (value >= MAX_DIST)
        return;
    if (map_is_wall(distmap, vec2i_to_f(point))) {
        // HACK: Put shine on walls
        set_normal_map_value(distmap, point, SHINE);
        // distmap.data[point.y * distmap.width + point.x] = SHINE;
        set_normal_map_value(illuminated, point, SHINE);
        return;
    };
    int data = distmap.data[point.y * distmap.width + point.x];
    if (dist_map_decode(data) <= value && dist_map_decode(data) >= 1)
        return;

    set_dist_map_value(distmap, point, value);
    // distmap.data[point.y * distmap.width + point.x] = dist_map_encode(value);
    illuminated.data[point.y * illuminated.width + point.x] =
        dist_map_encode(value);
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            Vec2i add = {i, j};
            dist_pass_recursive(distmap, vec2i_add(point, add), value + 1,
                                illuminated);
        }
    }
}

void dist_pass(Bitmap distmap, Vec2i point, Bitmap illuminated) {
    dist_pass_recursive(distmap, point, 1, illuminated);
}

void dist_reset(Bitmap distmap) {
    for (int x = 0; x < distmap.width; x++) {
        for (int y = 0; y < distmap.height; y++) {
            set_dist_map_value(distmap, (Vec2i){x, y}, 0);
        }
    }
}