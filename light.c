#include "light.h"
#include "camera.h"
#include "colors.h"
#include "objects.h"
#include <math.h>

void create_torches(Bitmap pixmap, Torch *torches, int num_torches) {
    for (int i = 0; i < num_torches; i++) {
        int x, y;
        do {
            x = random_between(0, pixmap.width);
            y = random_between(0, pixmap.height);
        } while (normal_map_decode(pixmap.data[y * pixmap.width + x]) !=
                 WALKABLE);

        torches[i] = (Torch){(Rect){{x, y}, {x, y}, 3}, random_between(3, 6)};
    }
}

void light_pass(WINDOW *win_game, Camera camera, Bitmap pixmap, Rect rect,
                int r, LightType t, Bitmap normalmap) {
    float inc = M_PI / 720.f;
    // Atualizar a posição da luz para levar em conta a posição da câmera
    Vec2f center = rect_center(rect);
    Vec2f light_pos_screen = {center.x - camera.x + 0.5f,
                              center.y - camera.y + 0.5f};
    // Vec2f light_pos_screen = {x - camera.x + 0.5f, y - camera.y + 0.5f};

    for (float theta = 0; theta < 2 * M_PI; theta += inc) {
        Vec2f vec = {cos(theta), sin(theta)};
        Vec2f line_pos = light_pos_screen;

        for (int k = 0; k < r; k++) {
            // add_term_line("%d\n", LIGHT_BASE + k);
            int value = LIGHT_RADIUS - 1 - r + k;
            if (t == LightType_Vision)
                value = k;

            if (line_pos.x < 0 || line_pos.x >= pixmap.width ||
                line_pos.y < 0 || line_pos.y >= pixmap.height)
                break;

            // wattrset(win_game, COLOR_PAIR(Culur_Light_Gradient + k));
            // print_pixel(win_game, line_pos.x, line_pos.y);
            set_light_map_value(pixmap, vec2f_to_i(line_pos), value + 1);

            if (!map_is_walkable(normalmap, camera, line_pos, vec)) {
                break;
            }
            line_pos = vec2f_add(line_pos, vec);
        }
    }
}

void light_reset(Bitmap distmap) {
    for (int x = 0; x < distmap.width; x++) {
        for (int y = 0; y < distmap.height; y++) {
            set_light_map_value(distmap, (Vec2i){x, y}, 0);
        }
    }
}