#pragma once
#include "term.h"
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>

typedef enum
{
    Color_Black,
    Color_Red,
    Color_Green,
    Color_Yellow,
    Color_Blue,
    Color_Magenta,
    Color_Cyan,
    Color_White,
    Culur_Black,
    Culur_Gray,
    Culur_Shine,
    Culur_Shine_Dimmed,
    Culur_Spike_Back,
    Culur_Spike_Front,
    Culur_Spike,
    Culur_Outer_Chest,
    Culur_Chest,
    Culur_Outer_Portal,
    Culur_Portal,
    Culur_Hotbar_Back,
    Culur_Hotbar_Front,
    Culur_Hotbar,
    Culur_Hotbar_Selected_Back,
    Culur_Hotbar_Selected_Front,
    Culur_Hotbar_Selected,
    Color__RGBSize,
    Culur_Default,
    Culur_Default_Gray,
    Culur_Default_Green,
    Culur_Light_Gradient,
    // ... LIGHT_RADIUS reserved
} Color;

void lerp_color(short start_color[3], short end_color[3], float t, short *result_color);
void init_gradient_color_pairs(short start_color[3], short end_color[3], int num_pairs, int base);
void setup_colors();
