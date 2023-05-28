#pragma once
#include "state.h"

#define MAX_TORCHES 10

#define MAX_LIGHT_CALC 30
#define MAX_LIGHT_SHINE 20
#define LIGHT_RADIUS MAX_LIGHT_SHINE // deprecated

typedef enum
{
    LightType_Vision,
    LightType_Torch
} LightType;

void light_pass(Bitmap pixmap, Rect rect, int r, Bitmap normalmap);
void light_reset(Bitmap distmap);
void create_torches(Bitmap pixmap, Torch *torches, int num_torches);
