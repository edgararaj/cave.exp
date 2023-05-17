#pragma once
#include "objects.h"

#define MAX_TORCHES 10
#define LIGHT_RADIUS 20

typedef struct {
    Rect position;
    int radius;
} Torch;

typedef enum { LightType_Vision, LightType_Torch } LightType;

void light_pass(WINDOW *win_game, Camera camera, Bitmap pixmap, Rect rect,
                int r, LightType t);
void light_reset(Bitmap distmap);