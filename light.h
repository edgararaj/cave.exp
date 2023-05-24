#pragma once
#include "state.h"

#define MAX_TORCHES 10
#define LIGHT_RADIUS 20


typedef enum
{
    LightType_Vision,
    LightType_Torch
} LightType;

void light_pass(Camera camera, Bitmap pixmap, Rect rect, int r, LightType t, Bitmap normalmap);
void light_reset(Bitmap distmap);
void create_torches(Bitmap pixmap, Torch *torches, int num_torches);
