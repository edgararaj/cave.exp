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
    Culur_Shine,
    Culur_Shine_Dimmed,
    Culur_Spike_Back,
    Culur_Spike_Front,
    Culur_Spike,
    Color__RGBSize,
    Culur_Default,
    Culur_Light_Gradient,
    // ... LIGHT_RADIUS reserved
} Color;

void lerp_color(short start_color[3], short end_color[3], float t, short *result_color);
void init_gradient_color_pairs(short start_color[3], short end_color[3], int num_pairs, int base);
void setup_colors();
