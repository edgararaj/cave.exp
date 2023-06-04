#pragma once
#include "objects.h"

#define MAX_DIST_CALC 30
#define MAX_DIST_SHINE 20

void dist_pass(Bitmap distmap, Bitmap collision, Vec2i point, Bitmap illuminated);
