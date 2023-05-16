#pragma once
#include "objects.h"

#define MAX_TORCHES 10
#define LIGHT_RADIUS 20

typedef struct {
    Rect position;
    int intensitiy;
} Torch;

void render_light(WINDOW *win_game, Camera camera, Bitmap pixmap, int x, int y,
                  int r, Bitmap *illuminated);