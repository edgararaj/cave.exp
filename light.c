#include "camera.h"
#include "objects.h"
#include <math.h>

#define MAX_TORCHES 10

void create_torches(Bitmap pixmap, Rect *torches, int num_torches) {

  for (int i = 0; i < num_torches; i++) {
    int x, y;
    do {
      x = rand() % pixmap.width;
      y = rand() % pixmap.height;
    } while (pixmap.data[y * pixmap.width + x] ==
             0); // Verifica se a tocha não está colocada numa parede

    torches[i] = (Rect){{x, y}, {x, y}, 3}; // 3 é a intensidade da luz
  }
}

int map_is_wall(Bitmap pixmap, Camera camera, Vec2f pos) {
  int data =
      pixmap.data[(int)(pos.y + camera.y) * pixmap.width + (int)(pos.x + camera.x)];
  return !data;
}

int map_is_walkable(Bitmap pixmap, Camera camera, Vec2f pos, Vec2f inc)
{
    Vec2f inc_x = {inc.x, 0}; 
    Vec2f inc_y = {0, inc.y};
    return (!map_is_wall(pixmap, camera, vec2f_add(pos, inc_x)) || !map_is_wall(pixmap, camera, vec2f_add(pos, inc_y))) && !map_is_wall(pixmap, camera, vec2f_add(pos, inc));
}

void render_light(WINDOW *win_game, Camera camera, Bitmap pixmap, int x, int y,
                  int r) {
  float inc = M_PI / 720.f;
  // Atualizar a posição da luz para levar em conta a posição da câmera
  Vec2f light_pos_screen = {x - camera.x + 0.5f, y - camera.y + 0.5f};

  wattrset(win_game, COLOR_PAIR(3));
  for (float theta = 0; theta < 2 * M_PI; theta += inc) {
    Vec2f vec = {cos(theta), sin(theta)};
    Vec2f line_pos = light_pos_screen;
    for (int k = 0; k < r; k++) {
      if (!map_is_walkable(pixmap, camera, line_pos, vec)) {
        break;
      }
      line_pos = vec2f_add(line_pos, vec);
      print_pixel(win_game, line_pos.x, line_pos.y);
    }
  }
}
