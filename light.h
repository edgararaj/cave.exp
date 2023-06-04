#pragma once
#include "state.h"

#define MAX_TORCHES 10

#define MAX_LIGHT_SHINE 20
#define LIGHT_RADIUS MAX_LIGHT_SHINE // deprecated

void light_pass(Bitmap pixmap, Rect rect, int r, Bitmap normalmap);
void create_torches(Bitmap pixmap, Torch *torches, int num_torches);
