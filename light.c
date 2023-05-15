#include "light.h"
#include "camera.h"
#include "objects.h"
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
    } while (pixmap.data[y * pixmap.width + x] == WALKABLE);

    torches[i] = (Torch){(Rect){{x, y}, {x, y}, 3}, random_between(3, 6)};
  }
}

void render_light(WINDOW *win_game, Camera camera, Bitmap pixmap, int x, int y,
                  int r, Bitmap *illuminated)
{
  float inc = M_PI / 720.f;
  // Atualizar a posição da luz para levar em conta a posição da câmera
  Vec2f light_pos_screen = {x - camera.x + 0.5f, y - camera.y + 0.5f};

  for (float theta = 0; theta < 2 * M_PI; theta += inc)
  {
    Vec2f vec = {cos(theta), sin(theta)};
    Vec2f line_pos = light_pos_screen;
    Vec2f abs_pos = {x, y};

    for (int k = 0; k < r; k++)
    {
      if (!map_is_walkable(pixmap, camera, line_pos, vec))
      {
        break;
      }
      line_pos = vec2f_add(line_pos, vec);
      abs_pos = vec2f_add(abs_pos, vec);
      if (illuminated)
        illuminated->data[(int)abs_pos.y * illuminated->width + (int)abs_pos.x] = 1;

      print_pixel(win_game, line_pos.x, line_pos.y);
    }
  }
}
