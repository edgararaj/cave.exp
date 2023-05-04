#include "objects.h"
#include "camera.h"
#include <math.h>

#define MAX_TORCHES 10

void create_torches(Bitmap pixmap, Rect* torches, int num_torches) {

    for (int i = 0; i < num_torches; i++) {
        int x, y;
        do {
            x = rand() % pixmap.width;
            y = rand() % pixmap.height;
        } while (pixmap.data[y * pixmap.width + x] == 0); // Ensure the torch is not placed in a wall

        torches[i] = (Rect){{x, y}, {x, y}, 3}; // 3 is the light intensity
    }
}

void render_light(WINDOW* win_game, Camera camera, Bitmap pixmap, int x, int y, int r)
{
    float inc = M_PI / 360.f;
    // Atualizar a posição da luz para levar em conta a posição da câmera
    Vec2f light_pos_screen = {
        x - camera.x + 0.5f,
        y - camera.y + 0.5f};

    wattrset(win_game, COLOR_PAIR(3));
    for (float theta = 0; theta < 2 * M_PI; theta += inc)
    {
        Vec2f vec = {
            cos(theta), sin(theta)};
        Vec2f line_pos = light_pos_screen;
        for (int k = 0; k < r; k++)
        {
            line_pos = vec2f_add(line_pos, vec);

            int data = pixmap.data[(int)(line_pos.y + camera.y) * pixmap.width + (int)(line_pos.x + camera.x)];
            if (!data)
            {
                break;
            }
            print_pixel(win_game, line_pos.x, line_pos.y);
        }
    }
}
