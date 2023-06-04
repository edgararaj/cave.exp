#include "light.h"
#include "camera.h"
#include "colors.h"
#include "map.h"
#include "objects.h"
#include "state.h"
#include "utils.h"
#include <math.h>

void create_torches(Bitmap pixmap, Torch *torches, int num_torches)
{
    for (int i = 0; i < num_torches; i++)
    {
        int x, y;
        do
        {
            x = random_between(0, pixmap.width);
            y = random_between(0, pixmap.height);
        } while (get_bitmap_value(pixmap, (Vec2i){x, y}) != WALKABLE);

        torches[i].position = (Rect){{x, y}, {x, y}, 3};
        torches[i].radius = random_between(6, 12);
    }
}

void light_pass(Bitmap pixmap, Rect rect, int r, Bitmap normalmap)
{
    // Atualizar a posição da luz para levar em conta a posição da câmera
    Vec2f center = rect_center(rect);

    float inc = M_PI / 720;
    for (float theta = 0; theta < 2 * M_PI; theta += inc)
    {
        Vec2f vec = {cos(theta), sin(theta)};
        Vec2f line_pos = center;

        for (int k = 0; k < r; k++)
        {
            if (line_pos.x < 0 || line_pos.x >= pixmap.width || line_pos.y < 0 || line_pos.y >= pixmap.height)
                break;

            set_bitmap_value(pixmap, vec2f_to_i(line_pos), r - k);

            if (!map_is_walkable(normalmap, line_pos, vec))
            {
                break;
            }
            line_pos = vec2f_add(line_pos, vec);
        }
    }
}
