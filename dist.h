#pragma once
#include "objects.h"

#define MAX_DIST_CALC 10
#define MAX_DIST_SHINE 5

void dist_pass(Bitmap distmap, Vec2i point, Bitmap illuminated);
void dist_reset(Bitmap distmap);
void shine_reset(Bitmap distmap);
void shine_pass(Bitmap normalmap, Vec2i point);