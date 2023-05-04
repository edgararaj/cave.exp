#include "camera.h"
#include "draw.h"
#include "objects.h"
#include "term.h"

void apply_horizontal_tunnel(int x1, int x2, int y, Bitmap bitmap) {
  int start = x1 < x2 ? x1 : x2;
  int end = x1 > x2 ? x1 : x2;

  for (int x = start; x <= end; x++) {
    bitmap.data[y * bitmap.width + x] = 1;
    if (y + 1 < bitmap.height) // Verifica se não estamos no limite do bitmap
    {
      bitmap.data[(y + 1) * bitmap.width + x] = 1;
    }
  }
}

void apply_vertical_tunnel(int y1, int y2, int x, Bitmap bitmap) {
  int start = y1 < y2 ? y1 : y2;
  int end = y1 > y2 ? y1 : y2;

  for (int y = start; y <= end; y++) {
    bitmap.data[y * bitmap.width + x] = 1;
    if (x + 1 < bitmap.width) // Verifica se não estamos no limite do bitmap
    {
      bitmap.data[y * bitmap.width + x + 1] = 1;
    }
  }
}

int radius_count(Bitmap bitmap, int x, int y, int r) {
  int result = 0;

  int xmin = MAX(x - r, 0);
  int xmax = MIN(x + r, bitmap.width - 1);
  for (int i = xmin; i <= xmax; i++) {
    int ymin = MAX(y - r, 0);
    int ymax = MIN(y + r, bitmap.height - 1);
    for (int j = ymin; j <= ymax; j++) {
      if (bitmap.data[j * bitmap.width + i])
        result++;
    }
  }
  return result;
}

void generate_tunnels_and_rasterize(Bitmap bitmap, Rect *rects,
                                    int rect_count) {
  bitmap_draw_rectangle(bitmap, rects[0]);
  for (int i = 1; i < rect_count; i++) {
    bitmap_draw_rectangle(bitmap, rects[i]);
    Vec2i prev_center = get_center(rects[i - 1]);
    Vec2i new_center = get_center(rects[i]);

    // add_term_line("P:%d,%d N:%d,%d", prev_center.x, prev_center.y,
    // new_center.x, new_center.y);

    if (rand() % 2 == 1) {
      apply_horizontal_tunnel(prev_center.x, new_center.x, prev_center.y,
                              bitmap);
      apply_vertical_tunnel(prev_center.y, new_center.y, new_center.x, bitmap);
    } else {
      apply_vertical_tunnel(prev_center.y, new_center.y, prev_center.x, bitmap);
      apply_horizontal_tunnel(prev_center.x, new_center.x, new_center.y,
                              bitmap);
    }
  }
}

int generate_rects(Rect window, Rect *rects, int rects_max) {
  int rects_count = 0;

  int div = 2;
  for (int f = 0; f < div * div; f++) {
    Rect sub = subdivide_rect(window, div, f);
    for (int i = 0; i < (rects_max / (div * div)); i++) {
      Rect new_rect;
      int valid_new_rect;
      for (int j = 0; j < 255; j++) {
        valid_new_rect = 1;
        new_rect = gen_random_subrect(sub);
        for (int n = 0; n < rects_count; n++) {
          if (collide_rect_rect(rects[n], expand_rect(new_rect, 2))) {
            valid_new_rect = 0;
            break;
          }
        }
        if (valid_new_rect)
          break;
      }
      if (valid_new_rect) {
        rects[rects_count++] = new_rect;
      }
    }
  }

  return rects_count;
}

void order_rects(Rect *rects, int rects_count) {
  for (int i = 0; i < rects_count; i++) {
    for (int j = 0; j < rects_count; j++) {
      if (rects[i].tl.x < rects[j].tl.x) {
        Rect temp = rects[i];
        rects[i] = rects[j];
        rects[j] = temp;
      }
    }
  }
}

void erode(Bitmap bitmap, int iterations) {
  // Crie um gerador de números aleatórios
  srand(time(NULL));

  // Erode com DLA
  for (int i = 0; i < iterations; i++) {
    // Encontre todas as células abertas
    int open_tiles[bitmap.width * bitmap.height];
    int open_tile_count = 0;

    for (int j = 0; j < bitmap.width * bitmap.height; j++) {
      if (bitmap.data[j] == 1) { // Supondo que 1 seja o código para '#'
        open_tiles[open_tile_count++] = j;
      }
    }

    // Escolha uma célula aberta aleatoriamente
    int random_index = rand() % open_tile_count;
    int digger = open_tiles[random_index];
    int digger_x = digger % bitmap.width;
    int digger_y = digger / bitmap.width;

    // Enquanto a célula escolhida ainda for um '#', escolha uma direção
    // aleatória e mova o "digger"
    while (bitmap.data[digger] == 1) {
      int direction = rand() % 4;

      switch (direction) {
      case 0: // Mova para a esquerda
        if (digger_x > 2) {
          digger_x -= 1;
        }
        break;
      case 1: // Mova para a direita
        if (digger_x < bitmap.width - 2) {
          digger_x += 1;
        }
        break;
      case 2: // Mova para cima
        if (digger_y > 2) {
          digger_y -= 1;
        }
        break;
      case 3: // Mova para baixo
        if (digger_y < bitmap.height - 2) {
          digger_y += 1;
        }
        break;
      }

      digger = digger_y * bitmap.width + digger_x;
    }

    // Marque a nova célula como um '#'
    bitmap.data[digger] = 1;
  }
}

void render_map(Camera camera, Bitmap map, WINDOW *window) {
  for (int x = 0; x < camera.width; ++x) {
    for (int y = 0; y < camera.height; ++y) {
      int map_x = x + camera.x;
      int map_y = y + camera.y;
      if (map.data[map_y * map.width + map_x] == 1) {
        print_pixel(window, x, y);
      }
    }
  }
}

void render_map_light(WINDOW *win_game, Camera camera, Bitmap pixmap, int x,
                      int y, int r, Bitmap map) {
  float inc = M_PI / 9000.f;
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

      if (map.data[(int)(line_pos.y * map.width + line_pos.x)]) {
        print_pixel(win_game, line_pos.x, line_pos.y);
      }
    }
  }
}
